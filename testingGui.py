import tkinter as tk
from tkinter import ttk

class ScrollableTable(tk.Frame):
    def __init__(self, master, headers, data):
        super().__init__(master)
        self.headers = headers
        self.data = data
        self.create_widgets()

    def create_widgets(self):
        # Create a treeview widget
        self.tree = ttk.Treeview(self, columns=self.headers, show="headings", selectmode="browse")

        # Add headers
        for header in self.headers:
            self.tree.heading(header, text=header)
            self.tree.column(header, anchor="center")

        # Insert data
        for row in self.data:
            self.tree.insert("", "end", values=row)

        # Create vertical scrollbar
        y_scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscroll=y_scrollbar.set)

        # Pack widgets
        y_scrollbar.pack(side="right", fill="y")
        self.tree.pack(expand=True, fill="both")

if __name__ == "__main__":
    # Sample data
    headers = ["Column 1", "Column 2"]
    data = [
        ("Row 1", "Data 1"),
        ("Row 2", "Data 2"),
        ("Row 3", "Data 3"),
        # Add more rows as needed
    ]

    # Create the main window
    root = tk.Tk()
    root.title("Scrollable Table")

    # Create and pack the scrollable table
    scrollable_table = ScrollableTable(root, headers, data)
    scrollable_table.pack(expand=True, fill="both")

    # Start the main loop
    root.mainloop()
