#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <pthread.h>
#include "../livefeaturecollector/LiveFeatureCollector.c"

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

enum{
  COL_FLOWID,
  COL_APP,
  COL_NPACK,
  NUM_COLS
};

GtkWidget *view_Capture;
GtkTreeModel *model_Capture;
GtkListStore  *store_Capture;

static GtkTreeModel * fill_model (gchar *flowid, gchar *app, gint *npack, GtkTreeIter iter, GtkListStore *store){

    /* Append a row and fill in some data */
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_FLOWID, flowid,
                        COL_APP, app,
                        COL_NPACK, npack,
                        -1);
    return GTK_TREE_MODEL (store);
};

static GtkWidget * create_view_from_model (GtkTreeModel *model){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;




    view = gtk_tree_view_new ();

    /* --- Column #1 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "ID",  
                                                renderer,
                                                "text", COL_FLOWID,
                                                NULL);

    /* --- Column #2 --- */

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Application",  
                                                renderer,
                                                "text", COL_APP,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Packets",  
                                                renderer,
                                                "text", COL_NPACK,
                                                NULL);                                            

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
    
    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model);

    return view;
};

void gtkToolBarStart_clicked(GtkWidget *widget, gpointer data) {
    pthread_t ptid; 
    pthread_create(&ptid, NULL, &scan, NULL);
    
}


int start (int   argc, char *argv[]){
    gchar *text;
    gint i;
    GtkTreeIter iter;

    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkWindow;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GObject *gtkScrolledWindow;
    GObject *gtkToolBarStart;
    GError *error = NULL;
    gtk_init (&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_file (builder, "main.ui", &error) == 0){
        g_printerr ("Error loading file: %s\n", error->message);
        g_clear_error (&error);
        return 1;
        }

    /* Connect signal handlers to the constructed widgets. */
    gtkWindow = gtk_builder_get_object (builder, "main");
    gtkToolBarStart = gtk_builder_get_object(builder, "gtkToolBarStart");
    

    store_Capture = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT);
    
    

    g_signal_connect (gtkToolBarStart, "clicked", gtkToolBarStart_clicked, NULL);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_widget_show_all(gtkWindow);

    
    gtk_main ();

    return 0;
}
