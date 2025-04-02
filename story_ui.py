import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import subprocess
import os
import tempfile

class StoryScriptUI:
    def __init__(self, root):
        self.root = root
        self.root.title("StoryScript Compiler")
        self.root.geometry("900x700")
        self.root.configure(bg="#f0f4f8")  # Light background color

        # Style configuration
        style = ttk.Style()
        style.configure("TNotebook", background="#f0f4f8", borderwidth=0)
        style.configure("TNotebook.Tab", font=("Helvetica", 12, "bold"), padding=[10, 5], 
                        background="#d1e0e6", foreground="#000000")  # Set font color to black
        style.map("TNotebook.Tab", background=[("selected", "#4a90e2")])  # Only change background, not foreground

        # Create notebook (tabs)
        self.notebook = ttk.Notebook(root)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=10)

        # Tab 1: Compiler
        self.tab1 = ttk.Frame(self.notebook, style="Custom.TFrame")
        self.notebook.add(self.tab1, text="Compiler")
        style.configure("Custom.TFrame", background="#f0f4f8")

        # Frame for input area
        input_frame = tk.Frame(self.tab1, bg="#f0f4f8", bd=2, relief="groove")
        input_frame.pack(pady=10, padx=10, fill="both", expand=True)

        tk.Label(input_frame, text="StoryScript Input:", font=("Arial", 14, "bold"), bg="#f0f4f8", fg="#333").pack(pady=5)
        self.input_text = scrolledtext.ScrolledText(input_frame, height=15, width=80, font=("Consolas", 11), 
                                                    bg="#ffffff", fg="#2c3e50", borderwidth=1, relief="flat")
        self.input_text.pack(pady=5, padx=5, fill="both", expand=True)

        # Compile and Run button
        button_frame = tk.Frame(self.tab1, bg="#f0f4f8")
        button_frame.pack(pady=10)
        self.run_button = tk.Button(button_frame, text="Compile and Run", command=self.compile_and_run, 
                                    font=("Arial", 12, "bold"), bg="#4a90e2", fg="white", 
                                    activebackground="#357abd", activeforeground="white", 
                                    relief="flat", padx=20, pady=5)
        self.run_button.pack()

        # Frame for output area
        output_frame = tk.Frame(self.tab1, bg="#f0f4f8", bd=2, relief="groove")
        output_frame.pack(pady=10, padx=10, fill="both", expand=True)

        tk.Label(output_frame, text="Output:", font=("Arial", 14, "bold"), bg="#f0f4f8", fg="#333").pack(pady=5)
        self.output_text = scrolledtext.ScrolledText(output_frame, height=15, width=80, font=("Consolas", 11), 
                                                     bg="#ffffff", fg="#2c3e50", borderwidth=1, relief="flat")
        self.output_text.pack(pady=5, padx=5, fill="both", expand=True)

        # Tab 2: User Manual
        self.tab2 = ttk.Frame(self.notebook, style="Custom.TFrame")
        self.notebook.add(self.tab2, text="User Manual")

        # Manual content frame
        manual_frame = tk.Frame(self.tab2, bg="#f0f4f8", bd=2, relief="groove")
        manual_frame.pack(pady=10, padx=10, fill="both", expand=True)

        tk.Label(manual_frame, text="User Manual", font=("Arial", 16, "bold"), bg="#f0f4f8", fg="#4a90e2").pack(pady=5)
        self.manual_text = scrolledtext.ScrolledText(manual_frame, height=35, width=80, font=("Arial", 11), 
                                                     bg="#ffffff", fg="#2c3e50", wrap="word", borderwidth=1, relief="flat")
        self.manual_text.pack(pady=5, padx=5, fill="both", expand=True)
        self.populate_manual()

    def compile_and_run(self):
        # Clear previous output
        self.output_text.delete(1.0, tk.END)

        # Get input from text area
        script = self.input_text.get(1.0, tk.END).strip()
        if not script:
            messagebox.showerror("Error", "Please enter a StoryScript program.")
            return

        # Write to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as temp_file:
            temp_file.write(script)
            temp_file_path = temp_file.name

        try:
            # Compile the C program if not already compiled
            if not os.path.exists("storyscript_compiler.exe"):
                self.output_text.insert(tk.END, "Compiling StoryScript compiler...\n")
                subprocess.run(["gcc", "-o", "storyscript_compiler.exe", "storyscript.c", "-lm"], check=True)
                self.output_text.insert(tk.END, "Compilation successful!\n\n")

            # Run the compiler with the temporary file
            result = subprocess.run(["storyscript_compiler.exe", temp_file_path], 
                                  capture_output=True, text=True, input="1\n2\n", timeout=10)
            
            # Display output
            self.output_text.insert(tk.END, result.stdout)
            if result.stderr:
                self.output_text.insert(tk.END, "\nErrors:\n" + result.stderr)
        except subprocess.CalledProcessError as e:
            messagebox.showerror("Compilation Error", f"Failed to compile: {e.stderr}")
        except subprocess.TimeoutExpired:
            messagebox.showerror("Timeout", "Execution timed out.")
        except Exception as e:
            messagebox.showerror("Error", str(e))
        finally:
            # Clean up temporary file
            os.unlink(temp_file_path)

    def populate_manual(self):
        manual_content = """
Introduction to StoryScript
-------------------------
StoryScript is a simple scripting language designed for creating interactive text-based stories. It supports narrative text, choices, inventory management, character states, and more, making it ideal for adventure games.

User Guidelines
---------------
1. Write commands in lowercase, followed by arguments in parentheses or brackets.
2. Use quotes for strings (e.g., "text").
3. Separate multiple arguments with commas.
4. Labels end with a colon (e.g., start:).
5. Ensure all labels referenced in choices or jumps are defined.

Commands and Syntax
-------------------
| Command                  | Syntax                                           | Description                                  |
|--------------------------|--------------------------------------------------|----------------------------------------------|
| title                    | title("text")                                   | Sets the story title                         |
| narrate                  | narrate("text")                                 | Displays narrative text                      |
| say                      | say("text")                                     | Displays dialogue                            |
| pause                    | pause(seconds)                                  | Pauses for specified seconds                 |
| choice                   | choice("opt1", "opt2") -> [label1, label2]      | Presents multiple choice options             |
| input                    | input("var")                                    | Prompts for integer input                    |
| confirm                  | confirm("text") -> [true_label, false_label]    | Yes/No prompt with branching                 |
| wait_for_key             | wait_for_key()                                  | Waits for Enter key press                    |
| goto                     | goto("label")                                   | Jumps to a label                             |
| end                      | end()                                           | Ends the story                               |
| return                   | return()                                        | Placeholder for subroutine return            |
| story_restart            | story_restart()                                 | Restarts the story                           |
| if                       | if("var") -> label                              | Jumps if variable is true                    |
| ifelse                   | ifelse("var") -> [true_label, false_label]      | Conditional branching                        |
| switch                   | switch("var") -> ["val1":label1, "val2":label2] | Multi-case branching                         |
| assign                   | assign("var", "value")                          | Assigns a value to a variable                |
| increase                 | increase("var", "amount")                       | Increases variable by amount                 |
| decrease                 | decrease("var", "amount")                       | Decreases variable by amount                 |
| scale                    | scale("var", "factor")                          | Multiplies variable by factor                |
| divide_by                | divide_by("var", "divisor")                     | Divides variable by divisor                  |
| randomize                | randomize("var", "range")                       | Sets variable to random value (0 to range-1) |
| both                     | both("var1", "var2")                            | Logical AND of two variables                 |
| either                   | either("var1", "var2")                          | Logical OR of two variables                  |
| invert                   | invert("var")                                   | Logical NOT of a variable                    |
| combine                  | combine("var", "str1", "str2")                  | Concatenates strings (numeric result)        |
| length_of                | length_of("var", "text")                        | Sets var to length of text                   |
| substring_in             | substring_in("var", "substr")                   | Checks if substr is in var                   |
| uppercase                | uppercase("var", "text")                        | Converts text to uppercase                   |
| lowercase                | lowercase("var", "text")                        | Converts text to lowercase                   |
| format_text              | format_text("var", "text")                      | Displays formatted text                      |
| create_inventory         | create_inventory("inv_name")                    | Creates an inventory                         |
| add_to_inventory         | add_to_inventory("inv_name", "item")            | Adds item to inventory                       |
| remove_item              | remove_item("inv_name", "item")                 | Removes item from inventory                  |
| has_item                 | has_item("inv_name", "item"[, "var"])           | Checks if item is in inventory               |
| count_inventory          | count_inventory("inv_name")                     | Counts items in inventory                    |
| clear_inventory          | clear_inventory("inv_name")                     | Clears all items from inventory              |
| show_inventory           | show_inventory("inv_name")                      | Displays inventory contents                  |
| create_scene             | create_scene("scene_name")                      | Placeholder for scene creation               |
| random_event             | random_event()                                  | Triggers a random event                      |
| set_background           | set_background("scene", "bg")                   | Sets scene background                        |
| trigger_scene            | trigger_scene("scene_name")                     | Triggers a scene                             |
| add_character            | add_character("name")                           | Adds a character                             |
| remove_character         | remove_character("name")                        | Removes a character                          |
| set_character_emotion    | set_character_emotion("name", "emotion")        | Sets character’s emotion                     |
| change_name              | change_name("old_name", "new_name")             | Renames a character                          |
| set_character_description| set_character_description("name", "desc")       | Sets character description                   |
| character_status         | character_status("name", "status")              | Sets character status                        |
| check_status             | check_status("name")                            | Displays character status                    |
| set_time_of_day          | set_time_of_day("time")                         | Sets current time                            |
| check_time_of_day        | check_time_of_day()                             | Displays current time                        |
"""
        self.manual_text.insert(tk.END, manual_content)
        self.manual_text.config(state="disabled")

if __name__ == "__main__":
    root = tk.Tk()
    app = StoryScriptUI(root)
    root.mainloop()