import os
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

# #Example
# sample_events = [
#     ("Event 1", "2023-11-01", "Location 1", "Description 1"),
#     ("Event 2", "2023-11-05", "Location 2", "Description 2"),
#     ("Event 3", "2023-11-10", "Location 3", "Description 3"),
#     ("Event 4", "2023-11-15", "Location 4", "Description 4"),
# ]
#
# # Insert 4 sample events
# for event in sample_events:
#     insert_event(*event)
#
# sample_guests = [
#     ("Guest 1", "guest1@example.com"),
#     ("Guest 2", "guest2@example.com"),
#     ("Guest 3", "guest3@examplcleare.com"),
#     ("Guest 4", "guest4@example.com"),
#     ("Guest 5", "guest5@example.com")
# ]
#
# # Insert 5 sample guests
# for guest in sample_guests:
#     insert_guest(*guest)

# cursor.execute("SELECT * FROM Events")
# events_data = cursor.fetchall()
#
# # Print the data
# for event in events_data:
#     print(event)
# print("-------------------------------------------------")
# Update data
# update_event(1, "Updated Event 1", "2023-11-02", "Updated Location 1", "Updated Description 1")
# update_event(2, "Updated Event 2", "2023-11-06", "Updated Location 2", "Updated Description 2")
# Print the data
# cursor.execute("SELECT * FROM Events")
# events_data = cursor.fetchall()
# for event in events_data:
#     print(event)

while True:
    i = input("Press 1 to add Event. \nPress 2 to add a Guest. \nPress 3 to Invite Guests.\nPress 4 to View Reports\nPress 5 to Update Event\nPress 0 to exit. \nResponse: ")
    i = int(i)
    os.system('clear')
    if i == 1:
        name = input("Event name: ")
        date = input("Event date: ")
        location = input("Event location: ")
        description = input("Event description: ")
        insert_event(name,date,location,description)
        os.system('clear')
        print("Event Added")
    elif i == 2:
        name = input("Guest name: ")
        email = input("Guest email: ")
        insert_guest(name, email)
        os.system('clear')
        print("Guest Added")
    elif i == 3:
        cursor.execute("SELECT * FROM Events")
        events_data = cursor.fetchall()
        for event in events_data:
            print(event)
        eve = input("Choose the Event Id: ")
        cursor.execute("SELECT * FROM Guests")
        guests = cursor.fetchall()
        for event in guests:
            print(event)
        gue = input("Choose the Guest Id: ")
        insert_rsvp(eve, gue, "NA")
        os.system('clear')
        print("RSVP done!")
    elif i == 4:
        print("----------REPORTS----------")
        while True:
            j = input("Press 1 for Events, 2 for Guests, 3 for RSVPS, 0 for Main Menu: ")
            j = int(j)
            if(j == 1) :
                os.system('clear')
                cursor.execute("SELECT * FROM Events")
                events_data = cursor.fetchall()
                for event in events_data:
                    print(event)
            elif(j==2) :
                os.system('clear')
                cursor.execute("SELECT * FROM Guests")
                guests = cursor.fetchall()
                for event in guests:
                    print(event)
            elif(j==3) :
                os.system('clear')
                cursor.execute("SELECT * FROM Events")
                events_data = cursor.fetchall()
                for event in events_data:
                    print(event)
                eveNumber = input("Enter the Event Number: ")
                os.system('clear')
                cursor.execute("SELECT g.name, g.email, r.response FROM RSVPs r JOIN Guests g ON g.guest_id = r.guest_id WHERE event_id=?", (eveNumber,))
                rsvps_data = cursor.fetchall()
                if len(rsvps_data) == 0:
                    print(f"No RSVPs found for event ID {eveNumber}")
                else:
                    print(f"RSVPs for event ID {eveNumber}:")
                    for rsvp in rsvps_data:
                        print(rsvp)
            elif j==0 :
                break
    elif i == 5:
        cursor.execute("SELECT * FROM Events")
        events_data = cursor.fetchall()
        for event in events_data:
            print(event)
        eveNumber2 = input("Enter the Event Number: ")
        name = input("New Event name: ")
        date = input("New Event date: ")
        location = input("New Event location: ")
        description = input("New Event description: ")
        update_event(eveNumber2, name, date, location, description)
    elif i == 0:
        break

conn.close()
