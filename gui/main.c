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

enum{
  COL_FLOWID,
  COL_APP,
  COL_NPACK,
  NUM_COLS
};

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


int main (int   argc, char *argv[]){
    gchar *text;
    gint i;
    GtkTreeIter    iter;
    GtkListStore  *store;

    GtkWidget *row;
    GtkBuilder *builder;
    GObject *gtkToolbar;
    GObject *gtkToolButton;
    GObject *gtkWindow;
    GObject *gtkListBox;
    GObject *gtkLabelLogo;
    GObject *gtkScrolledWindow;
    GError *error = NULL;
    GtkWidget *view;
    GtkTreeModel *model;
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
    

    store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT);
    
    for(i = 0; i < 400; i++)
    {
        if (i % 10 == 0)
        {

            model = fill_model ("flowid","Youtube",i,iter,store);

            view = create_view_from_model (model);

            gtkScrolledWindow =  gtk_builder_get_object(builder, "gtkScrolledWindow");

            gtk_container_add (GTK_CONTAINER (gtkScrolledWindow), view);

            gtk_widget_show(gtkScrolledWindow);
        }
    //gtk_grid_attach ((GtkGrid *)gtkGrid, view, 0, 1, 1, 1);
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

    /*if (gtk_widget_get_no_show_all(gtkWindow)){
        gtk_widget_hide (gtkWindow);
    }*/
    }
    gtk_widget_show_all(gtkWindow);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_main ();

    return 0;
}
