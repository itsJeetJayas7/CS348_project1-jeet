import sqlite3

# Connect to the database (or create a new one)
conn = sqlite3.connect('event_RSVP.db')

# Create a cursor object
cursor = conn.cursor()

# Create a table
cursor.execute('''
    CREATE TABLE IF NOT EXISTS Events (
        event_id INTEGER PRIMARY KEY,
        title TEXT,
        date DATE,
        location TEXT,
        description TEXT
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS Guests (
        guest_id INTEGER PRIMARY KEY,
        name TEXT,
        email TEXT
    )
''')

cursor.execute('''
    CREATE TABLE IF NOT EXISTS RSVPs (
        rsvp_id INTEGER PRIMARY KEY,
        event_id INTEGER,
        guest_id INTEGER,
        response TEXT,
        FOREIGN KEY (event_id) REFERENCES Events (event_id),
        FOREIGN KEY (guest_id) REFERENCES Guests (guest_id)
    )
''')


# Commit the changes and close the connection
conn.commit()

def insert_event(title, date, location, description):
    cursor.execute("INSERT INTO Events (title, date, location, description) VALUES (?, ?, ?, ?)", (title, date, location, description))
    conn.commit()

# Insert data into the "Guests" table
def insert_guest(name, email):
    cursor.execute("INSERT INTO Guests (name, email) VALUES (?, ?)", (name, email))
    conn.commit()

# Insert data into the "RSVPs" table
def insert_rsvp(event_id, guest_id, response):
    cursor.execute("INSERT INTO RSVPs (event_id, guest_id, response) VALUES (?, ?, ?)", (event_id, guest_id, response))
    conn.commit()

def update_event(event_id, title, date, location, description):
    cursor.execute("UPDATE Events SET title=?, date=?, location=?, description=? WHERE event_id=?", (title, date, location, description, event_id))
    conn.commit()

# Update data in the "Guests" table
def update_guest(guest_id, name, email):
    cursor.execute("UPDATE Guests SET name=?, email=? WHERE guest_id=?", (name, email, guest_id))
    conn.commit()

# Update data in the "RSVPs" table based on event_id and guest_id
def update_rsvp(event_id, guest_id, new_response):
    cursor.execute("UPDATE RSVPs SET response=? WHERE event_id=? AND guest_id=?", (new_response, event_id, guest_id))
    conn.commit()

def delete_event(event_id):
    cursor.execute("DELETE FROM Events WHERE event_id=?", (event_id,))
    conn.commit()

# Delete a record from the "Guests" table by guest_id
def delete_guest(guest_id):
    cursor.execute("DELETE FROM Guests WHERE guest_id=?", (guest_id,))
    conn.commit()

# Delete an RSVP record from the "RSVPs" table based on event_id and guest_id
def delete_rsvp(event_id, guest_id):
    cursor.execute("DELETE FROM RSVPs WHERE event_id=? AND guest_id=?", (event_id, guest_id))
    conn.commit()

#Example
sample_events = [
    ("Event 1", "2023-11-01", "Location 1", "Description 1"),
    ("Event 2", "2023-11-05", "Location 2", "Description 2"),
    ("Event 3", "2023-11-10", "Location 3", "Description 3"),
    ("Event 4", "2023-11-15", "Location 4", "Description 4"),
    ("Event 5", "2023-11-20", "Location 5", "Description 5"),
    ("Event 6", "2023-11-25", "Location 6", "Description 6"),
    ("Event 7", "2023-11-30", "Location 7", "Description 7"),
    ("Event 8", "2023-12-05", "Location 8", "Description 8"),
    ("Event 9", "2023-12-10", "Location 9", "Description 9"),
    ("Event 10", "2023-12-15", "Location 10", "Description 10")
]

# Insert 10 sample events
for event in sample_events:
    insert_event(*event)


cursor.execute("SELECT * FROM Events")
events_data = cursor.fetchall()

# Print the data
for event in events_data:
    print(event)
print("-------------------------------------------------")
# Update data
update_event(1, "Updated Event 1", "2023-11-02", "Updated Location 1", "Updated Description 1")
update_event(2, "Updated Event 2", "2023-11-06", "Updated Location 2", "Updated Description 2")
# Print the data
cursor.execute("SELECT * FROM Events")
events_data = cursor.fetchall()
for event in events_data:
    print(event)




conn.close()
