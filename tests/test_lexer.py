"""对真实命令行程序验证记号、统计、错误位置及恢复后的输出。"""
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

EXECUTABLE = str(Path(sys.argv.pop(1)).resolve())


class LexerTests(unittest.TestCase):
    def scan(self, source):
        if isinstance(source, str):
            source = source.encode("utf-8")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "source.txt"
            path.write_bytes(source)
            completed = subprocess.run(
                [EXECUTABLE, str(path)], capture_output=True,
                encoding="utf-8", timeout=5,
            )
        self.assertIn(completed.returncode, (0, 1), completed.stderr)
        self.assertEqual(completed.returncode, int(bool(completed.stderr)))
        tokens = []
        for line in completed.stdout.split("\n统计结果：")[0].splitlines()[1:]:
            match = re.match(r"(\w+)\s+(\d+)\s+(\d+)\s*(.*)", line)
            if match:
                kind, row, column, word = match.groups()
                tokens.append((kind, int(row), int(column), word.rstrip()))
        stats = {}
        for line in completed.stdout.split("\n统计结果：")[1].strip().splitlines():
            key, value = line.rsplit(": ", 1)
            stats[key] = int(value)
        errors = [(int(row), int(column)) for row, column in
                  re.findall(r"第(\d+)行 第(\d+)列", completed.stderr)]
        self.assertEqual(stats["字符总数（字节，含空白和注释）"], len(source))
        self.assertEqual(stats["词法错误数"], len(errors))
        self.assertEqual(stats["有效单词总数（不含 EOF）"], len(tokens) - 1)
        self.assertEqual(tokens[-1][0], "Eof")
        return tokens, stats, errors

    def test_tokens_and_statistics(self):
        source = 'int x=12; // comment\nfloat y=.5e+2; char c=\'a\'; "hi"\n'
        tokens, stats, errors = self.scan(source)
        self.assertEqual(errors, [])
        self.assertEqual([(t[0], t[3]) for t in tokens], [
            ("KwInt", "int"), ("Identifier", "x"), ("Assign", "="),
            ("IntLiteral", "12"), ("Semicolon", ";"), ("KwFloat", "float"),
            ("Identifier", "y"), ("Assign", "="), ("FloatLiteral", ".5e+2"),
            ("Semicolon", ";"), ("KwChar", "char"), ("Identifier", "c"),
            ("Assign", "="), ("CharLiteral", "'a'"), ("Semicolon", ";"),
            ("StringLiteral", '"hi"'), ("Eof", ""),
        ])
        for name, count in {"关键字": 3, "标识符": 3, "整数常量": 1,
                            "浮点常量": 1, "字符常量": 1, "字符串常量": 1,
                            "运算符": 3, "界符": 3, "注释个数": 1,
                            "源程序行数（物理行）": 2,
                            "代码行数（含有效记号）": 2}.items():
            self.assertEqual(stats[name], count, name)

    def test_multiple_errors_and_recovery(self):
        tokens, _, errors = self.scan('@ $\n1e+; 123abc; 1.2.3;\n"bad\\q";\n\'ab\';\nint ok=7;')
        self.assertEqual(errors, [(1, 1), (1, 3), (2, 1), (2, 6), (2, 14), (3, 5), (4, 1)])
        self.assertEqual([t[3] for t in tokens[-6:-1]], ["int", "ok", "=", "7", ";"])
        self.assertEqual(sum(t[0] == "Semicolon" for t in tokens), 6)

    def test_unclosed_quotes_recover_at_newline(self):
        tokens, _, errors = self.scan('"bad\n\'x\n"bad\\\nint ok;')
        self.assertEqual(errors, [(1, 1), (2, 1), (3, 1)])
        self.assertIn(("Identifier", 4, 5, "ok"), tokens)

    def test_character_length_and_multiple_escapes(self):
        tokens, stats, errors = self.scan(r"'' 'ab' 'a' '\n' '\q' " + r'"\q\z"' + '; ok')
        self.assertEqual(len(errors), 5)
        self.assertEqual(stats["字符常量"], 2)
        self.assertEqual(tokens[-2][3], "ok")

    def test_comments_and_embedded_comment_markers(self):
        tokens, stats, errors = self.scan('/* @ \' \n */ int a; // $ "\n"/*x*/";')
        self.assertEqual(errors, [])
        self.assertEqual(stats["注释个数"], 2)
        self.assertEqual(stats["代码行数（含有效记号）"], 2)
        self.assertIn(("KwInt", 2, 5, "int"), tokens)
        self.assertIn(("StringLiteral", 3, 1, '"/*x*/"'), tokens)

    def test_unclosed_comment_at_eof(self):
        tokens, stats, errors = self.scan('@ int a; /* never closed\n $')
        self.assertEqual(errors, [(1, 1), (1, 10)])
        self.assertEqual(stats["注释个数"], 1)
        self.assertEqual(tokens[-1][1:3], (2, 3))

    def test_newline_conventions(self):
        for ending in (b"\n", b"\r", b"\r\n"):
            for trailing in (b"", ending):
                with self.subTest(ending=ending, trailing=trailing):
                    tokens, stats, errors = self.scan(b"int a;" + ending + b"@ b;" + trailing)
                    self.assertEqual(stats["源程序行数（物理行）"], 2)
                    self.assertEqual(errors, [(2, 1)])
                    self.assertIn(("Identifier", 2, 3, "b"), tokens)

    def test_empty_and_whitespace(self):
        for source, lines in (("", 0), ("\n", 1), ("\r\n", 1), ("\n\n", 2), (" \t", 1)):
            tokens, stats, errors = self.scan(source)
            self.assertEqual(stats["源程序行数（物理行）"], lines)
            self.assertEqual(stats["代码行数（含有效记号）"], 0)
            self.assertEqual(len(tokens), 1)
            self.assertEqual(errors, [])

    def test_nul_does_not_stop_scanning(self):
        tokens, _, errors = self.scan(b"a\0b;\n//\0 comment\nint c;")
        self.assertEqual(errors, [(1, 2)])
        self.assertIn(("Identifier", 1, 3, "b"), tokens)
        self.assertIn(("Identifier", 3, 5, "c"), tokens)

    def test_preprocessor_spacing_and_position(self):
        tokens, stats, errors = self.scan(' /* c */ # define X 1\n#bogus int y;\na #define z\n')
        self.assertEqual(errors, [(2, 1), (3, 3)])
        self.assertEqual(stats["预处理指令"], 1)
        self.assertIn(("PpDefine", 1, 10, "# define"), tokens)
        self.assertIn(("Identifier", 2, 12, "y"), tokens)

    def test_maximal_munch(self):
        tokens, _, errors = self.scan('a>>=2; p->*q; x++!=y && z<=1 || x::y.*z; ()[]{},:?~')
        self.assertEqual(errors, [])
        for kind in ("ShrAssign", "ArrowStar", "PlusPlus", "Ne", "And", "Le", "Or",
                     "Scope", "DotStar", "RParen", "RBracket", "Colon", "Question"):
            self.assertIn(kind, [t[0] for t in tokens])

    def test_errors_at_eof(self):
        for source in ('@', '1e+', "'", '"\\', '/*', '#bogus', "'\\n"):
            with self.subTest(source=source):
                _, _, errors = self.scan(source)
                self.assertEqual(errors, [(1, 1)])


if __name__ == "__main__":
    unittest.main()
