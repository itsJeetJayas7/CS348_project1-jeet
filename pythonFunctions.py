from flask import Flask, render_template

app = Flask(__name__)

# Define the Python function to execute
def my_python_function():
    # Your Python code here
    return "Hello from Python!"

# Create a route to call the Python function
@app.route('/execute_python_function', methods=['GET'])
def execute_python_function():
    text_data = "Hello from Flask!"  # Replace with the text you want to send
    return text_data

if __name__ == '__main__':
    app.run(debug=True)
