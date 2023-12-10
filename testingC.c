#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <gtk/gtk.h>

sqlite3 *db;

typedef struct {
    int event_id;
    char *title;
    char *date;
    char *location;
    char *description;
} Event;

GtkBuilder * builder;
GtkBuilder * builder_t;
gboolean t = FALSE;
Event event_t[1000];
int index_data;

void connectDB() {
    int rc = sqlite3_open("event_RSVP.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
}

void createTables() {
    char *sql_events = "CREATE TABLE IF NOT EXISTS Events ("
                       "event_id INTEGER PRIMARY KEY,"
                       "title TEXT,"
                       "date DATE,"
                       "location TEXT,"
                       "description TEXT);";

    char *sql_guests = "CREATE TABLE IF NOT EXISTS Guests ("
                       "guest_id INTEGER PRIMARY KEY,"
                       "name TEXT,"
                       "email TEXT);";

    char *sql_rsvps = "CREATE TABLE IF NOT EXISTS RSVPs ("
                      "rsvp_id INTEGER PRIMARY KEY,"
                      "event_id INTEGER,"
                      "guest_id INTEGER,"
                      "response TEXT,"
                      "FOREIGN KEY (event_id) REFERENCES Events (event_id),"
                      "FOREIGN KEY (guest_id) REFERENCES Guests (guest_id));";

    sqlite3_exec(db, sql_events, 0, 0, 0);
    sqlite3_exec(db, sql_guests, 0, 0, 0);
    sqlite3_exec(db, sql_rsvps, 0, 0, 0);
}

void printEventsTable() {
    printf("-------- Events Table --------\n");
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM Events;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    index_data = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("Event ID: %d, Title: %s, Date: %s, Location: %s, Description: %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3),
               sqlite3_column_text(stmt, 4));
        
        event_t[index_data].event_id = sqlite3_column_int(stmt, 0);
        event_t[index_data].title = strdup((const char *) sqlite3_column_text(stmt, 1));
        event_t[index_data].date = strdup((const char *) sqlite3_column_text(stmt, 2));
        event_t[index_data].location = strdup((const char *) sqlite3_column_text(stmt, 3));
        event_t[index_data++].description = strdup((const char *) sqlite3_column_text(stmt, 4));
    }

    sqlite3_finalize(stmt);
    printf("------------------------------\n");
}

void insertEvent(char *title, char *date, char *location, char *description) {
    char *sql = "INSERT INTO Events (title, date, location, description) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, location, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, description, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void insertGuest(char *name, char *email) {
    char *sql = "INSERT INTO Guests (name, email) VALUES (?, ?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void insertRSVP(int event_id, int guest_id, char *response) {
    char *sql = "INSERT INTO RSVPs (event_id, guest_id, response) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, event_id);
    sqlite3_bind_int(stmt, 2, guest_id);
    sqlite3_bind_text(stmt, 3, response, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Update function for Events table
void updateEvent(int event_id, char *title, char *date, char *location, char *description) {
    char *sql = "UPDATE Events SET title=?, date=?, location=?, description=? WHERE event_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, location, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, event_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Update function for Guests table
void updateGuest(int guest_id, char *name, char *email) {
    char *sql = "UPDATE Guests SET name=?, email=? WHERE guest_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, guest_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Update function for RSVPs table
void updateRSVP(int event_id, int guest_id, char *new_response) {
    char *sql = "UPDATE RSVPs SET response=? WHERE event_id=? AND guest_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, new_response, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, event_id);
    sqlite3_bind_int(stmt, 3, guest_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Delete function for Events table
void deleteEvent(int event_id) {
    char *sql = "DELETE FROM Events WHERE event_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, event_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Delete function for Guests table
void deleteGuest(int guest_id) {
    char *sql = "DELETE FROM Guests WHERE guest_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, guest_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Delete function for RSVPs table
void deleteRSVP(int event_id, int guest_id) {
    char *sql = "DELETE FROM RSVPs WHERE event_id=? AND guest_id=?";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, event_id);
    sqlite3_bind_int(stmt, 2, guest_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void event_table_set() {

    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder, "storeTree"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_e"));
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col3"));
    GtkTreeViewColumn * cm4 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col4"));
    GtkTreeViewColumn * cm5 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col5"));
    
    GtkCellRenderer * cmr1 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr2 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr3 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr4 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr5 = gtk_cell_renderer_text_new();

    gtk_tree_view_column_pack_start(cm1, cmr1, TRUE);
	gtk_tree_view_column_pack_start(cm2, cmr2, TRUE);
    gtk_tree_view_column_pack_start(cm3, cmr3, TRUE);
    gtk_tree_view_column_pack_start(cm4, cmr4, TRUE);
    gtk_tree_view_column_pack_start(cm5, cmr5, TRUE);

    gtk_tree_view_column_add_attribute(cm1, cmr1, "text", 0);
	gtk_tree_view_column_add_attribute(cm2, cmr2, "text", 1);
    gtk_tree_view_column_add_attribute(cm3, cmr3, "text", 2);
    gtk_tree_view_column_add_attribute(cm4, cmr4, "text", 3);
    gtk_tree_view_column_add_attribute(cm5, cmr5, "text", 4);
  
  
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "select"));

    GtkTreeIter iter;

    for (int i = 0; i < index_data; i++) {
        //printf("here");
        char id_t[10];
        sprintf(id_t, "%d",
            event_t[i].event_id);
        // printf("%s %s %s %s %s\n",id_t, name_t, date_t, loc_t, disc_t);
        // printf("here");
        gtk_tree_store_append(treeStore_mm, &iter, NULL);
	    gtk_tree_store_set(treeStore_mm, &iter, 0, id_t, -1);
	    gtk_tree_store_set(treeStore_mm, &iter, 1, event_t[i].title, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 2, event_t[i].date, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 3, event_t[i].location, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 4, event_t[i].description, -1);
    }

    selection_mm = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView_mm));
}

int getEventId(const char *eventName, const char *eventDate) {
    sqlite3 *db;
    if (sqlite3_open("event_RSVP.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Error opening the database\n");
        return -1;  // Return -1 to indicate an error
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT event_id FROM Events WHERE title = '%s' AND date = '%s'", eventName, eventDate);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing the query\n");
        sqlite3_close(db);
        return -1;  // Return -1 to indicate an error
    }

    int eventId = -1;  // Initialize to -1 (indicating not found)

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Read the event_id from the result
        eventId = sqlite3_column_int(stmt, 0);
    }

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return eventId;
}

void insert_random_guests(sqlite3 *db, int num_guests) {
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    const char *sql = "INSERT INTO Guests (name, email) VALUES (?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Error preparing SQL statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Array of sample names and emails (you can replace these with your own data)
    const char *sample_names[] = {"Alice", "Bob", "Charlie", "David", "Eva"};
    const char *sample_emails[] = {"alice@example.com", "bob@example.com", "charlie@example.com", "david@example.com", "eva@example.com"};

    // Insert random guest data
    for (int i = 0; i < num_guests; ++i) {
        int random_index = rand() % 5; // Change this if you have more names/emails
        const char *name = sample_names[random_index];
        const char *email = sample_emails[random_index];

        // Bind parameters to the prepared statement
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

        // Execute the statement
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Error inserting data: %s\n", sqlite3_errmsg(db));
        }

        // Reset the statement for the next iteration
        sqlite3_reset(stmt);
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

void on_submit_event_clicked() {
    GtkEntry *entry1 = GTK_ENTRY(gtk_builder_get_object(builder, "entry_name"));
    GtkEntry *entry2 = GTK_ENTRY(gtk_builder_get_object(builder, "entry_date"));
    GtkEntry *entry3 = GTK_ENTRY(gtk_builder_get_object(builder, "entry_loc"));
    GtkEntry *entry4 = GTK_ENTRY(gtk_builder_get_object(builder, "entry_disc"));

    //char name[128], date[128], loc[128], disc[256];
    char * name = (char *) gtk_entry_get_text(entry1);
    char * date = (char *) gtk_entry_get_text(entry2);
    char * loc = (char *) gtk_entry_get_text(entry3);
    char * disc = (char *) gtk_entry_get_text(entry4);

    insertEvent(name, date, loc, disc);
    printEventsTable();

    gtk_editable_delete_text(GTK_EDITABLE(entry1), 0 ,-1);
    gtk_editable_delete_text(GTK_EDITABLE(entry2), 0 ,-1);
    gtk_editable_delete_text(GTK_EDITABLE(entry3), 0 ,-1);
    gtk_editable_delete_text(GTK_EDITABLE(entry4), 0 ,-1);

    
    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder, "storeTree"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_e"));
    gtk_tree_store_clear(treeStore_mm);
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col3"));
    GtkTreeViewColumn * cm4 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col4"));
    GtkTreeViewColumn * cm5 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col5"));
    
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "select"));

    GtkTreeIter iter;

    for (int i = 0; i < index_data; i++) {
        
        char id_t[10];
        sprintf(id_t, "%d",
            event_t[i].event_id);
        // printf("%s %s %s %s %s\n",id_t, name_t, date_t, loc_t, disc_t);
        
        gtk_tree_store_append(treeStore_mm, &iter, NULL);
	    gtk_tree_store_set(treeStore_mm, &iter, 0, id_t, -1);
	    gtk_tree_store_set(treeStore_mm, &iter, 1, event_t[i].title, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 2, event_t[i].date, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 3, event_t[i].location, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 4, event_t[i].description, -1);
    }
}



void guests_done() {

    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder_t, "ts"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder_t, "tv"));
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "col1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "col2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "col3"));
    
    GtkCellRenderer * cmr1 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr2 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr3 = gtk_cell_renderer_text_new();
    
    gtk_tree_view_column_pack_start(cm1, cmr1, TRUE);
	gtk_tree_view_column_pack_start(cm2, cmr2, TRUE);
    gtk_tree_view_column_pack_start(cm3, cmr3, TRUE);
    
    gtk_tree_view_column_add_attribute(cm1, cmr1, "text", 0);
	gtk_tree_view_column_add_attribute(cm2, cmr2, "text", 1);
    gtk_tree_view_column_add_attribute(cm3, cmr3, "text", 2);
    
  
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder_t, "select"));

    GtkTreeIter iter;

    sqlite3 *db;
    if (sqlite3_open("event_RSVP.db", &db) != SQLITE_OK) {
        g_warning("Error opening the database");
        return;
    }

    // Prepare and execute the SELECT query
    const char *query = "SELECT * FROM Guests";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        g_warning("Error preparing the query");
        sqlite3_close(db);
        return;
    }

    // Fetch data and populate the GtkTreeStore
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int guestId = sqlite3_column_int(stmt, 0);
        const char *guestName = (const char *)sqlite3_column_text(stmt, 1);
        const char *guestEmail = (const char *)sqlite3_column_text(stmt, 2);
        char id_t[10];
        sprintf(id_t, "%d",
            guestId);
        gtk_tree_store_append(treeStore_mm, &iter, NULL);
	    gtk_tree_store_set(treeStore_mm, &iter, 0, id_t, -1);
	    gtk_tree_store_set(treeStore_mm, &iter, 1, guestName, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 2, guestEmail, -1);
    }

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    selection_mm = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView_mm));
}

int event_id_t;

void print_guests_for_event(int eventId) {
    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder_t, "tStore"));
    GtkTreeIter iter;
    sqlite3 *db;
    int rc = sqlite3_open("event_RSVP.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_stmt *stmt;

    // Prepare SQL query to fetch guests and their responses for the given event ID
    const char *query = "SELECT Guests.name, Guests.email, RSVPs.response FROM Guests "
                        "INNER JOIN RSVPs ON Guests.guest_id = RSVPs.guest_id "
                        "WHERE RSVPs.event_id = ?";

    // Prepare the statement
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind the event ID parameter
        sqlite3_bind_int(stmt, 1, eventId);

        // Execute the query
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *name = (const char *)sqlite3_column_text(stmt, 0);
            const char *email = (const char *)sqlite3_column_text(stmt, 1);
            const char *response = (const char *)sqlite3_column_text(stmt, 2);

            // Print guest information

            char * temp_name = strdup(name);
            char * temp_email = strdup(email);
            char * temp_response = strdup(response);
            if(temp_name == NULL || temp_email == NULL || temp_response == NULL || treeStore_mm == NULL) {
                g_print("There is something NULL\n");
            }
            gtk_tree_store_append(treeStore_mm, &iter, NULL);
	        gtk_tree_store_set(treeStore_mm, &iter, 0, (char*) temp_name, -1);
	        gtk_tree_store_set(treeStore_mm, &iter, 1, (char*) temp_email, -1);
            gtk_tree_store_set(treeStore_mm, &iter, 2, (char*) temp_response, -1);
        }

        // Finalize the statement
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "Error preparing SQL statement: %s\n", sqlite3_errmsg(db));
    }

    //sqlite3_finalize(stmt);
    //sqlite3_close(db);
}

void rsvps_done(int eventId) {

    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder_t, "ts2"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder_t, "tv2"));
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "c1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "c2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder_t, "c3"));
    
    GtkCellRenderer * cmr1 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr2 = gtk_cell_renderer_text_new();
    GtkCellRenderer * cmr3 = gtk_cell_renderer_text_new();
    
    gtk_tree_view_column_pack_start(cm1, cmr1, TRUE);
	gtk_tree_view_column_pack_start(cm2, cmr2, TRUE);
    gtk_tree_view_column_pack_start(cm3, cmr3, TRUE);
    
    gtk_tree_view_column_add_attribute(cm1, cmr1, "text", 0);
	gtk_tree_view_column_add_attribute(cm2, cmr2, "text", 1);
    gtk_tree_view_column_add_attribute(cm3, cmr3, "text", 2);
    
  
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder_t, "select"));

    GtkTreeIter iter;

    print_guests_for_event(event_id_t);

    
}

void on_delete_e_clicked() {
    gboolean temp = t;
    t = FALSE;
    // GtkEntry *entry1 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_title"));
    // GtkEntry *entry2 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_date"));
    // GtkEntry *entry3 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_loc"));
    // GtkEntry *entry4 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_desc"));

    // //char name[128], date[128], loc[128], disc[256];
    // char * name = (char *) gtk_entry_get_text(entry1);
    // char * date = (char *) gtk_entry_get_text(entry2);
    // char * loc = (char *) gtk_entry_get_text(entry3);
    // char * disc = (char *) gtk_entry_get_text(entry4);

    // printf("%s %s %s %s", name, date, loc, disc);

    deleteEvent(event_id_t);
    printEventsTable();

    // gtk_editable_delete_text(GTK_EDITABLE(entry1), 0 ,-1);
    // gtk_editable_delete_text(GTK_EDITABLE(entry2), 0 ,-1);
    // gtk_editable_delete_text(GTK_EDITABLE(entry3), 0 ,-1);
    // gtk_editable_delete_text(GTK_EDITABLE(entry4), 0 ,-1);

    
    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder, "storeTree"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_e"));
    gtk_tree_store_clear(treeStore_mm);
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col3"));
    GtkTreeViewColumn * cm4 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col4"));
    GtkTreeViewColumn * cm5 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col5"));
    
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "select"));

    GtkTreeIter iter;

    for (int i = 0; i < index_data; i++) {
        
        char id_t[10];
        sprintf(id_t, "%d",
            event_t[i].event_id);
        // printf("%s %s %s %s %s\n",id_t, name_t, date_t, loc_t, disc_t);
        
        gtk_tree_store_append(treeStore_mm, &iter, NULL);
	    gtk_tree_store_set(treeStore_mm, &iter, 0, id_t, -1);
	    gtk_tree_store_set(treeStore_mm, &iter, 1, event_t[i].title, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 2, event_t[i].date, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 3, event_t[i].location, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 4, event_t[i].description, -1);
    }

    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(builder_t, "event_w"));    
    gtk_widget_destroy(window);

    GtkWidget * switch_widget = GTK_WIDGET(gtk_builder_get_object(builder, "switch"));
    gtk_switch_set_active(GTK_SWITCH(switch_widget), FALSE);
    
}

void on_update_e_clicked() {
    gboolean temp = t;
    t = FALSE;
    GtkEntry *entry1 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_title"));
    GtkEntry *entry2 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_date"));
    GtkEntry *entry3 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_loc"));
    GtkEntry *entry4 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_desc"));

    //char name[128], date[128], loc[128], disc[256];
    char * name = (char *) gtk_entry_get_text(entry1);
    char * date = (char *) gtk_entry_get_text(entry2);
    char * loc = (char *) gtk_entry_get_text(entry3);
    char * disc = (char *) gtk_entry_get_text(entry4);

    updateEvent(event_id_t, name, date, loc, disc);
    printEventsTable();
    gtk_editable_delete_text(GTK_EDITABLE(entry1), 0 ,-1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), strdup(name));
    gtk_editable_delete_text(GTK_EDITABLE(entry2), 0 ,-1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), strdup(date));
    gtk_editable_delete_text(GTK_EDITABLE(entry3), 0 ,-1);
    gtk_editable_delete_text(GTK_EDITABLE(entry4), 0 ,-1);
    
    
    
    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder, "storeTree"));
    GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_e"));
    gtk_tree_store_clear(treeStore_mm);
    GtkTreeViewColumn * cm1 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col1"));
    GtkTreeViewColumn * cm2 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col2"));
    GtkTreeViewColumn * cm3 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col3"));
    GtkTreeViewColumn * cm4 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col4"));
    GtkTreeViewColumn * cm5 = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col5"));
    
    GtkTreeSelection * selection_mm = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "select"));

    GtkTreeIter iter;

    for (int i = 0; i < index_data; i++) {
        
        char id_t[10];
        sprintf(id_t, "%d",
            event_t[i].event_id);
        // printf("%s %s %s %s %s\n",id_t, name_t, date_t, loc_t, disc_t);
        
        gtk_tree_store_append(treeStore_mm, &iter, NULL);
	    gtk_tree_store_set(treeStore_mm, &iter, 0, id_t, -1);
	    gtk_tree_store_set(treeStore_mm, &iter, 1, event_t[i].title, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 2, event_t[i].date, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 3, event_t[i].location, -1);
        gtk_tree_store_set(treeStore_mm, &iter, 4, event_t[i].description, -1);
    }
    GtkWidget * switch_widget = GTK_WIDGET(gtk_builder_get_object(builder, "switch"));
    gtk_switch_set_active(GTK_SWITCH(switch_widget), FALSE);
}

void on_select_changed(GtkWidget * c) {
    if(t) {
    gchar * value1;
    gchar * value2;
    gchar * value3;

    GtkTreeIter iter;
    GtkTreeModel * model;    

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(c), &model, &iter) == FALSE) {
            return;
    }

    gtk_tree_model_get(model, &iter, 0, &value1, -1);
    
    gtk_tree_model_get(model, &iter, 1, &value2, -1);
    
    gtk_tree_model_get(model, &iter, 2, &value3, -1);
    
    char procInfo[1000];
    builder_t = gtk_builder_new();
    gtk_builder_add_from_file(builder_t, "w.glade", NULL);

    GtkEntry *entry1 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_title"));
    GtkEntry *entry2 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_date"));
    GtkEntry *entry3 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_loc"));
    GtkEntry *entry4 = GTK_ENTRY(gtk_builder_get_object(builder_t, "entry_new_desc"));
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1), value2);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2), value3);
    //print_guests_for_event(atoi(value1));
    event_id_t = atoi(value1);

    // Get the main window widget
    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(builder_t, "event_w"));
    //g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder_t, NULL);
    
    guests_done();
    
    rsvps_done(event_id_t);

    // Show the main window
    gtk_widget_show_all(window);
    }
}

void on_switch_state_set(GtkSwitch * s) {
     t = gtk_switch_get_active(s);
    //  if(t) {
    //     g_print("its Active!");
    //  } else {
    //     g_print("not Active");
    //  }
}

void show_popup_message(GtkWindow *parent_window, const gchar *message) {
    GtkMessageType type = GTK_MESSAGE_INFO;
    GtkButtonsType buttons = GTK_BUTTONS_OK;

    // Create a message dialog
    GtkWidget *dialog = gtk_message_dialog_new(parent_window,
                                               GTK_DIALOG_MODAL,
                                               type,
                                               buttons,
                                               "%s",
                                               message);

    // Set the title of the dialog (optional)
    gtk_window_set_title(GTK_WINDOW(dialog), "Popup Message");

    // Run the dialog and wait for user response
    gtk_dialog_run(GTK_DIALOG(dialog));

    // Destroy the dialog
    gtk_widget_destroy(dialog);
}

void on_select2_changed(GtkWidget * c) {
    gchar * value1;
    gchar * value2;
    gchar * value3;

    GtkTreeIter iter;
    GtkTreeModel * model;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(c), &model, &iter) == FALSE) {
            return;
    }
    
    gtk_tree_model_get(model, &iter, 0, &value1, -1);
    
    gtk_tree_model_get(model, &iter, 1, &value2, -1);
    
    gtk_tree_model_get(model, &iter, 2, &value3, -1);
    
    char procInfo[1000];
    
    insertRSVP(event_id_t, atoi(value1), "NA");

    sprintf(procInfo, "%s Added and RSVPed!", value2);

    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(builder_t, "event_w"));
    show_popup_message((GtkWindow *) window, procInfo);
    
    GtkTreeStore * treeStore_mm = GTK_TREE_STORE(gtk_builder_get_object(builder_t, "tStore"));
    //GtkTreeView * treeView_mm = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_e"));
    gtk_tree_store_clear(treeStore_mm);
    print_guests_for_event(event_id_t);
}

int main(int argc, char *argv[]) {
    connectDB();
    createTables();
    printEventsTable();
    char title[256], date[256], location[256], description[256];
    int ID;
    gtk_init(&argc, &argv);

    // Load UI from Glade file
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "cs348_t.glade", NULL);

    // Get the main window widget
    GtkWidget * window = GTK_WIDGET(gtk_builder_get_object(builder, "main_w"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    event_table_set();

    // Show the main window
    gtk_widget_show_all(window);

    // Run the GTK main loop
    gtk_main();
    sqlite3_close(db);
    return 0;

    // printf("Event ID: ");
    // scanf("%d", &ID);
    // printf("Event name: ");
    // scanf("%s", title);
    // printf("Event date: ");
    // scanf("%s", date);
    // printf("Event location: ");
    // scanf("%s", location);
    // printf("Event description: ");
    // scanf("%s", description);

    // deleteEvent(ID);

    // printf("After Insert:\n");
    // printEventsTable();

    
    return 0;
}
