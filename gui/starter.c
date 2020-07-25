#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

// static GtkWidget * create_row (const gchar *text){
//   GtkWidget *row, *handle, *box, *label, *image;

//   row = gtk_list_box_row_new ();

//   box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
//   g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);

//   handle = gtk_event_box_new ();
//   gtk_container_add (GTK_CONTAINER (box), handle);

//   label = gtk_label_new (text);
//   gtk_container_add_with_properties (GTK_CONTAINER (box), label, "expand", TRUE, NULL);
//   return row;
// }

// enum {
//     COL_INDEX = 0,
//     COL_TYPE,
//     COL_TX_PACKETS,
//     COL_RX_PACKETS,
//     COL_TX_BYTES,
//     COL_RX_BYTES,
// };

enum{
  COL_NAME = 0,
  COL_AGE,
  NUM_COLS
};

static GtkTreeModel * create_and_fill_model (void){
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

    /* Append a row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Heinz El-Mann",
                        COL_AGE, 51,
                        -1);

    /* append another row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Jane Doe",
                        COL_AGE, 23,
                        -1);

    /* ... and a third row */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_NAME, "Joe Bungop",
                        COL_AGE, 91,
                        -1);

    return GTK_TREE_MODEL (store);
};

static GtkWidget * create_view_and_model (void){
    
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    GtkWidget           *view;

    view = gtk_tree_view_new ();

    /* --- Column #1 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Name",  
                                                renderer,
                                                "text", COL_NAME,
                                                NULL);

    /* --- Column #2 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Age",  
                                                renderer,
                                                "text", COL_AGE,
                                                NULL);

    model = create_and_fill_model ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    return view;
};


int main (int   argc, char *argv[]){
    gchar *text;
    gint i;
    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkWindow;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GObject *gtkGrid;
    GError *error = NULL;
    GtkWidget *view;
    gtk_init (&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "starter.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
        }

    /* Connect signal handlers to the constructed widgets. */
    gtkWindow = gtk_builder_get_object (builder, "starter");
    gtkGrid =  gtk_builder_get_object(builder, "gtkGrid");
    view = create_view_and_model ();
    // gtk_container_add (GTK_CONTAINER (gtkGrid), view);
    gtk_grid_attach ((GtkGrid *)gtkGrid, view, 1, 2, 1, 1);
    // gtk_list_box_set_selection_mode ( gtkListBox, GTK_SELECTION_NONE);
    // for (i = 0; i < 20; i++)
    // {
    //   text = g_strdup_printf ("Row %d", i);
    //   // row = create_row(text);
    //   printf("%s hhhhh \n", text);
    //   gtk_list_box_insert (gtkListBox, gtkLabelLogo, -1);
    // }
    // GtkToolbar = gtk_builder_get_object(builder, "toolBar");
    // GtkToolButton = gtk_builder_get_object(builder, "toolButtonAllow");
    // g_signal_connect (GtkToolButton, "clicked", G_CALLBACK (print_hello), NULL);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_widget_show_all (gtkWindow);
    gtk_main ();

    return 0;
}
