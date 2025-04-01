import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import subprocess
import os
import time

COMPILER_EXEC = "storyscript_compiler.exe"
STORY_FILE = "story.txt"
OUTPUT_FILE = "output.target"

class StoryScriptIDE:
    def __init__(self, root):
        self.root = root
        self.root.title("StoryScript IDE")
        self.root.geometry("1200x800")
        self.root.configure(bg="#2e2e2e")  # Dark background for the window

        self.create_menu()
        self.create_widgets()

    def create_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_file)
        file_menu.add_command(label="Open", command=self.load_file)
        file_menu.add_command(label="Save", command=self.save_file)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

        run_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Run", menu=run_menu)
        run_menu.add_command(label="Compile", command=self.compile_code)
        run_menu.add_command(label="Run Story", command=self.run_story)

        help_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Help", menu=help_menu)
        help_menu.add_command(label="User Manual", command=self.show_manual)

    def create_widgets(self):
        main_frame = tk.Frame(self.root, bg="#2e2e2e")  # Dark background for main frame
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self.code_input = scrolledtext.ScrolledText(main_frame, wrap=tk.WORD, font=("Consolas", 12), bg="#1e1e1e", fg="white")
        self.code_input.pack(fill=tk.BOTH, expand=True, side=tk.LEFT, padx=(0, 5))

        button_frame = tk.Frame(main_frame, bg="#2e2e2e")  # Dark background for button frame
        button_frame.pack(fill=tk.X, pady=5)

        # Using basic tk.Button for custom styles
        tk.Button(button_frame, text="Compile", command=self.compile_code, bg="#4CAF50", fg="white", font=("Helvetica", 10)).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Run", command=self.run_story, bg="#4CAF50", fg="white", font=("Helvetica", 10)).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Load", command=self.load_file, bg="#4CAF50", fg="white", font=("Helvetica", 10)).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Save", command=self.save_file, bg="#4CAF50", fg="white", font=("Helvetica", 10)).pack(side=tk.LEFT, padx=5)

        self.output_terminal = scrolledtext.ScrolledText(main_frame, height=10, font=("Courier", 10), bg="#1e1e1e", fg="white")
        self.output_terminal.pack(fill=tk.BOTH, expand=True, pady=(5, 0))

    def new_file(self):
        self.code_input.delete("1.0", tk.END)
        self.output_terminal.delete("1.0", tk.END)

    def compile_code(self):
        code = self.code_input.get("1.0", tk.END).strip()
        if not code:
            messagebox.showwarning("Warning", "No code to compile!")
            return

        with open(STORY_FILE, "w") as f:
            f.write(code)

        try:
            result = subprocess.run([COMPILER_EXEC, STORY_FILE], capture_output=True, text=True)
            self.output_terminal.delete("1.0", tk.END)
            self.output_terminal.insert(tk.END, result.stdout)
            if result.stderr:
                self.output_terminal.insert(tk.END, f"\nErrors:\n{result.stderr}")
            if result.returncode == 0:
                messagebox.showinfo("Success", "Compilation successful!")
            else:
                messagebox.showerror("Error", "Compilation failed!")
        except FileNotFoundError:
            messagebox.showerror("Error", f"Compiler '{COMPILER_EXEC}' not found!")

    def run_story(self):
        if not os.path.exists(OUTPUT_FILE):
            messagebox.showerror("Error", "Compile the story first!")
            return

        self.output_terminal.delete("1.0", tk.END)
        self.output_terminal.insert(tk.END, "--- Running Story ---\n\n")

        with open(OUTPUT_FILE, "r") as f:
            lines = f.readlines()

        for line in lines:
            line = line.strip()
            if line.startswith("PRINT"):
                text = line.split('"')[1]
                self.output_terminal.insert(tk.END, f"{text}\n")
            elif line.startswith("PROMPT"):
                choices = line.split("->")[0].replace("PROMPT", "").strip().split('", "')
                choices = [c.strip('"') for c in choices]
                labels = line.split("->")[1].strip()[1:-1].split(", ")
                choice = self.show_choice_dialog(choices)
                self.output_terminal.insert(tk.END, f"You chose: {choices[choice-1]}\n")
                for i, l in enumerate(lines):
                    if l.strip() == f"LABEL {labels[choice-1]}":
                        lines = lines[i:]
                        break
            elif line.startswith("HALT"):
                self.output_terminal.insert(tk.END, "The story ends here.\n")
                break
            self.output_terminal.update()
            time.sleep(0.1)

    def show_choice_dialog(self, options):
        dialog = tk.Toplevel(self.root)
        dialog.title("Make a Choice")
        dialog.geometry("300x200")
        dialog.configure(bg="#2e2e2e")

        tk.Label(dialog, text="Choose an option:", bg="#2e2e2e", fg="white").pack(pady=5)
        choice_var = tk.IntVar(value=1)

        for i, opt in enumerate(options, 1):
            tk.Radiobutton(dialog, text=opt, variable=choice_var, value=i, bg="#2e2e2e", fg="white").pack(anchor="w", padx=10)

        tk.Button(dialog, text="Confirm", command=dialog.destroy, bg="#4CAF50", fg="white").pack(pady=10)
        dialog.wait_window()
        return choice_var.get()

    def load_file(self):
        path = filedialog.askopenfilename(filetypes=[("StoryScript Files", "*.txt"), ("All Files", "*.*")])
        if path:
            with open(path, "r") as f:
                self.code_input.delete("1.0", tk.END)
                self.code_input.insert(tk.END, f.read())

    def save_file(self):
        path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("StoryScript Files", "*.txt"), ("All Files", "*.*")])
        if path:
            with open(path, "w") as f:
                f.write(self.code_input.get("1.0", tk.END))
            messagebox.showinfo("Success", "File saved!")

    def show_manual(self):
        manual = """
        StoryScript IDE - User Manual
        ================================
        - **title("Text")**: Set story title.
        - **say("Text")**: Display dialogue.
        - **narrate("Text")**: Display narration.
        - **pause(seconds)**: Delay execution.
        - **choice("Opt1", "Opt2") -> [label1, label2]**: Present choices.
        - **input(var)**: Capture user input.
        - **confirm("Text") -> [yes, no]**: Yes/No prompt.
        - **goto(label)**: Jump to label.
        - **story_restart()**: Restart story.
        - **end()**: End story.
        - **if(condition) -> label**: Conditional jump.
        - **assign(var, value)**: Set variable.
        - **increase(var, value)**: Increase variable.
        - **combine(str1, str2)**: Combine strings.
        - **create_inventory(var)**: Create inventory.
        - **add_to_inventory(var, item)**: Add item.
        - **show_inventory()**: Display inventory.
        """
        self.output_terminal.delete("1.0", tk.END)
        self.output_terminal.insert(tk.END, manual)

if __name__ == "__main__":
    root = tk.Tk()
    app = StoryScriptIDE(root)
    root.mainloop()
