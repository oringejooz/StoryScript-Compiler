import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import subprocess
import os

# Paths
COMPILER_EXECUTABLE = "storyscript_compiler.exe"  # Update if needed for Windows with storyscript_compiler.exe
STORY_FILE = "story.txt"
OUTPUT_FILE = "output.target"

# Compile the StoryScript code
# Compile the StoryScript code
def compile_code():
    code = code_input.get("1.0", tk.END).strip()

    if not code:
        messagebox.showwarning("Warning", "No code provided to compile!")
        return

    # Save the code to story.txt
    with open(STORY_FILE, "w") as f:
        f.write(code)

    # Run the compiler
    compile_cmd = [COMPILER_EXECUTABLE, STORY_FILE]
    
    try:
        result = subprocess.run(compile_cmd, capture_output=True, text=True)
        output_terminal.delete("1.0", tk.END)
        output_terminal.insert(tk.END, result.stdout)

        if result.returncode == 0:
            messagebox.showinfo("Success", "Compilation successful!")
        else:
            messagebox.showerror("Error", f"Compilation failed:\n{result.stderr}")
            output_terminal.insert(tk.END, f"\n[ERROR]: {result.stderr}")
    except FileNotFoundError:
        messagebox.showerror("Error", "Compiler not found! Check the path to 'storyscript_compiler.exe'.")
    except Exception as e:
        messagebox.showerror("Error", f"Failed to run compiler: {e}")

# Run the compiled story
def run_story():
    if not os.path.exists(OUTPUT_FILE):
        messagebox.showerror("Error", "No compiled target file found. Compile first!")
        return

    try:
        with open(OUTPUT_FILE, "r") as f:
            lines = f.readlines()

        output_terminal.delete("1.0", tk.END)
        output_terminal.insert(tk.END, "--- Running Story ---\n\n")

        for line in lines:
            line = line.strip()
            
            # Display narration
            if line.startswith("narrate("):
                content = line.split("narrate(", 1)[1].rstrip(")")
                output_terminal.insert(tk.END, content + "\n\n")

            # Handle choices
            elif line.startswith("choice("):
                options = line.split("choice(", 1)[1].rstrip(")").split(", ")
                option_text = [opt.strip('"') for opt in options]
                
                choice_result = simple_choice(option_text)
                output_terminal.insert(tk.END, f"You chose: {option_text[choice_result - 1]}\n\n")

            # Handle jump to labels
            elif line.startswith("goto("):
                label_name = line.split("goto(", 1)[1].rstrip(")")
                output_terminal.insert(tk.END, f"Jumping to {label_name}...\n\n")

            # End story
            elif line.startswith("end()"):
                output_terminal.insert(tk.END, "The story ends here.\n")
                break

        messagebox.showinfo("Story Completed", "The story has run successfully!")
    except Exception as e:
        messagebox.showerror("Error", f"Failed to run the story: {e}")


def simple_choice(options):
    """Simulate user choice for simple story."""
    choice_window = tk.Toplevel(root)
    choice_window.title("Make a Choice")
    tk.Label(choice_window, text="Make a choice:").pack(pady=5)

    choice_var = tk.IntVar()
    choice_var.set(1)  # Default to first choice

    for idx, option in enumerate(options, start=1):
        tk.Radiobutton(choice_window, text=option, variable=choice_var, value=idx).pack(anchor="w")

    def confirm_choice():
        choice_window.destroy()

    tk.Button(choice_window, text="Confirm", command=confirm_choice).pack(pady=10)
    choice_window.wait_window()

    return choice_var.get()


# Load a StoryScript file
def load_file():
    file_path = filedialog.askopenfilename(filetypes=[("StoryScript Files", "*.txt"), ("All Files", "*.*")])
    if file_path:
        with open(file_path, "r") as f:
            code_input.delete("1.0", tk.END)
            code_input.insert(tk.END, f.read())

# Save the current StoryScript to a file
def save_file():
    file_path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("StoryScript Files", "*.txt"), ("All Files", "*.*")])
    if file_path:
        with open(file_path, "w") as f:
            f.write(code_input.get("1.0", tk.END))
        messagebox.showinfo("Saved", "File saved successfully!")

# Display the User Manual
# Display the User Manual
def show_manual():
    manual_text = """
    🎮 StoryScript User Manual 🎮
    ===========================

🚀 Introduction
StoryScript is a scripting language used to create interactive, text-based adventure games. 
It allows you to control the flow of the story, present choices to the player, and implement 
basic inventory and text manipulation.

📚 Basic Syntax Overview
===========================
Each statement in StoryScript follows this format:

1. title("Title of the Story") - Displays the story title.
2. narrate("Narration text here...") - Shows a description or narrative to the player.
3. choice("Option 1", "Option 2") -> [label1, label2] - Prompts the user with choices and redirects to labels.
4. label_name: - Creates a label to define sections of the story.
5. goto(label_name) - Jumps to another label.
6. end() - Ends the story.

🎭 Advanced Story Control Commands
===================================
1. add_item("item_name") - Add item to inventory.
2. remove_item("item_name") - Remove item from inventory.
3. show_inv() - Show inventory.
4. combine("item1", "item2") - Combine items.
5. length("text") - Get length of text.
6. uppercase("text") - Convert text to uppercase.
7. lowercase("text") - Convert text to lowercase.
8. format_text("template", "value") - Format text.
9. set_time("HH:MM") - Set time.
10. check_time() - Get current time.

===========================
Happy Story Crafting! 🎯
    """
    manual_display.delete("1.0", tk.END)
    manual_display.insert("1.0", manual_text)

# Main window setup
root = tk.Tk()
root.title("StoryScript Compiler IDE")
root.geometry("1920x1080")

# Create Tabs
tab_control = ttk.Notebook(root)
tab1 = ttk.Frame(tab_control)
tab2 = ttk.Frame(tab_control)
tab_control.add(tab1, text="Editor & Console")
tab_control.add(tab2, text="User Manual")
tab_control.pack(expand=1, fill="both")

### --- TAB 1: Editor & Console --- ###
# Frame for Code Input and Buttons
frame_top = tk.Frame(tab1)
frame_top.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

# Code Input
code_input = tk.Text(frame_top, wrap="word", font=("Consolas", 12))
code_input.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=(0, 5))

# Scrollbar
scrollbar_code = tk.Scrollbar(frame_top, command=code_input.yview)
scrollbar_code.pack(side=tk.RIGHT, fill="y")
code_input.config(yscrollcommand=scrollbar_code.set)

# Frame for Buttons
frame_buttons = tk.Frame(tab1)
frame_buttons.pack(fill=tk.X, padx=5, pady=5)

# Compile, Run, Load, and Save Buttons
btn_compile = tk.Button(frame_buttons, text="Compile", command=compile_code, bg="#4CAF50", fg="white")
btn_compile.pack(side=tk.LEFT, padx=5)

btn_run = tk.Button(frame_buttons, text="Run", command=run_story, bg="#008CBA", fg="white")
btn_run.pack(side=tk.LEFT, padx=5)

btn_load = tk.Button(frame_buttons, text="Load", command=load_file)
btn_load.pack(side=tk.LEFT, padx=5)

btn_save = tk.Button(frame_buttons, text="Save", command=save_file)
btn_save.pack(side=tk.LEFT, padx=5)

# Output Terminal
output_terminal = tk.Text(tab1, height=10, bg="black", fg="white", font=("Courier", 10))
output_terminal.pack(fill=tk.BOTH, expand=True, padx=5, pady=(5, 5))

### --- TAB 2: User Manual --- ###
# User Manual
manual_display = tk.Text(tab2, wrap="word", font=("Arial", 12))
manual_display.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

# Load User Manual
show_manual()

# Run the Tkinter event loop
root.mainloop()
