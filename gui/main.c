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
    COL_INDEX_SCAN,
    COL_FLOWID,
    COL_APP,
    COL_NPACK,
    NUM_COLS
};

GObject *gtkScrolledWindow;

GtkWidget *view_Capture;
GtkTreeModel *model_Capture;
GtkListStore  *store_Capture;
GtkTreeIter iter_Capture;

GtkAdjustment * adj;

GObject *gtkToolBarStart;
GObject *gtkToolBarStop;
GObject *gtkToolBarRestart;

int col_index = 0;

pthread_t ptid_scan; 

void gtkStoreAppend(gchar *data){
    char * token = strtok(data, "|");
    gtk_list_store_append (store_Capture , &iter_Capture);
    gtk_list_store_set (store_Capture, &iter_Capture,
            COL_INDEX_SCAN, col_index,
            COL_FLOWID, token,
            COL_APP, strtok(NULL, "|"),
            COL_NPACK, 40,
            -1);

    // adj = gtk_scrolled_window_get_vadjustment (gtkScrolledWindow);
    // gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
    col_index++;
    
}

static GtkTreeModel * create_and_fill_model_scan (void){
    
    store_Capture = gtk_list_store_new (NUM_COLS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING , G_TYPE_INT);

    // gtk_list_store_append (store_Capture , &iter_Capture);
    // gtk_list_store_set (store_Capture, &iter_Capture,
    //         COL_FLOWID, "token",
    //         COL_APP, "djamel",
    //         COL_NPACK, "999",
    //         -1);

    return GTK_TREE_MODEL (store_Capture);
};

static GtkWidget * create_view_and_model_scan (void){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N°",  
                                                renderer,
                                                "text", COL_INDEX_SCAN,
                                                NULL);
    
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "FlowID",  
                                                renderer,
                                                "text", COL_FLOWID,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Type",  
                                                renderer,
                                                "text", COL_APP,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° Pkts",  
                                                renderer,
                                                "text", COL_NPACK,
                                                NULL);


    model_Capture = create_and_fill_model_scan ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model_Capture);

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model_Capture);

    return view;
};


void gtkToolBarStart_clicked(GtkWidget *widget, gpointer data) {
    // gtk_list_store_append (store_Capture , &iter_Capture);
    // gtk_list_store_set (store_Capture, &iter_Capture,
    //         COL_FLOWID, "token",
    //         COL_APP, "djamel",
    //         COL_NPACK, "999",
    //         -1);

    gtk_widget_set_sensitive(widget, FALSE);
    gtk_widget_set_sensitive(gtkToolBarStop, TRUE);
    pthread_create(&ptid_scan, NULL, &scan, NULL);
    
}
void gtkToolBarStop_clicked(GtkWidget *widget, gpointer data) {
    
    gtk_widget_set_sensitive(widget, FALSE);
    gtk_widget_set_sensitive(gtkToolBarStart, TRUE);
    pthread_cancel(ptid_scan);
}

void gtkToolBarRestart_clicked(GtkWidget *widget, gpointer data) {
    gtk_list_store_clear (store_Capture);
    col_index = 0;
    gtkToolBarStop_clicked(gtkToolBarStop, data);
    gtkToolBarStart_clicked(gtkToolBarStart, data);
    

}

int start (int   argc, char *argv[]){
    gchar *text;
    gint i;
    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkWindow;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
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
    gtkToolBarStop = gtk_builder_get_object(builder, "gtkToolBarStop");
    gtkToolBarRestart = gtk_builder_get_object(builder, "gtkToolBarRestart");
    gtkScrolledWindow = gtk_builder_get_object(builder,"gtkScrolledWindow");

    
    
    view_Capture =  create_view_and_model_scan ();
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow), view_Capture);
    gtk_widget_set_sensitive(gtkToolBarStop, FALSE);

    g_signal_connect (gtkToolBarStart, "clicked", gtkToolBarStart_clicked, NULL);
    g_signal_connect (gtkToolBarStop, "clicked", gtkToolBarStop_clicked, NULL);
    g_signal_connect (gtkToolBarRestart, "clicked", gtkToolBarRestart_clicked, NULL);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    

    gtk_widget_show_all(gtkWindow);

    
    gtk_main ();

    return 0;
}
