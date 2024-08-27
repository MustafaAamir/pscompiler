#!/usr/bin/python

import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QTextEdit, QPushButton, QTextEdit
import regex as re
from PyQt6.QtGui import QColor, QPalette
from PyQt6.QtCore import Qt
import subprocess
import tempfile

class CompilerGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
    def initUI(self):
        self.setWindowTitle('PseudoCompiler')
        self.setGeometry(100, 100, 1200, 800)

        # Main widget and layout
        main_widget = QWidget(self)
        main_layout = QHBoxLayout(main_widget)

        self.editor = QTextEdit()
        self.editor.setStyleSheet("""
            background-color: #282828;
            color: #ebdbb2;
            font-family: 'Monospace';
            font-size: 16px;
        """)

        self.output = QTextEdit()
        self.output.setReadOnly(True)
        self.output.setStyleSheet("""
            background-color: #1d2021;
            color: #ebdbb2;
            font-family: 'Courier New';
            font-size: 14px;
        """)

        compile_button = QPushButton('Compile')
        compile_button.setStyleSheet("""
            background-color: #98971a;
            color: #282828;
            font-weight: bold;
            padding: 5px;
        """)
        compile_button.clicked.connect(self.compile_code)

        left_layout = QVBoxLayout()
        left_layout.addWidget(self.editor)
        left_layout.addWidget(compile_button)

        main_layout.addLayout(left_layout)
        main_layout.addWidget(self.output)

        self.setCentralWidget(main_widget)

        palette = self.palette()
        palette.setColor(QPalette.ColorRole.Window, QColor('#282828'))
        palette.setColor(QPalette.ColorRole.WindowText, QColor('#ebdbb2'))
        palette.setColor(QPalette.ColorRole.Base, QColor('#282828'))
        palette.setColor(QPalette.ColorRole.AlternateBase, QColor('#3c3836'))
        palette.setColor(QPalette.ColorRole.ToolTipBase, QColor('#282828'))
        palette.setColor(QPalette.ColorRole.ToolTipText, QColor('#ebdbb2'))
        palette.setColor(QPalette.ColorRole.Text, QColor('#ebdbb2'))
        palette.setColor(QPalette.ColorRole.Button, QColor('#504945'))
        palette.setColor(QPalette.ColorRole.ButtonText, QColor('#ebdbb2'))
        palette.setColor(QPalette.ColorRole.BrightText, QColor('#fb4934'))
        palette.setColor(QPalette.ColorRole.Highlight, QColor('#458588'))
        palette.setColor(QPalette.ColorRole.HighlightedText, QColor('#ebdbb2'))
        self.setPalette(palette)

    def compile_code(self):
        code = self.editor.toPlainText()
        with tempfile.NamedTemporaryFile(mode='w', suffix='.pseudo', delete=False) as temp_file:
            temp_file.write(code)
            temp_file_name =  temp_file.name;

        try:
            process = subprocess.Popen(['./pscompiler', temp_file_name],
                                       stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE,
                                       universal_newlines=True)
            stdout, stderr = process.communicate()

            output = f"{stdout}\n\n{stderr}"
            self.output.setPlainText(output)
        except subprocess.CalledProcessError as e:
                self.output.setPlainText(f"Compilation failed: {e}")
        except FileNotFoundError:
            self.output.setPlainText("Compiler not found. Please check your compiler installation.")
        finally:
            import os
            os.unlink(temp_file_name)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = CompilerGUI()
    ex.show()
    sys.exit(app.exec())
