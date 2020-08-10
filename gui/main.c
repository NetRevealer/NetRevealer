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
enum AppUsage_Cols {
    COL_AU_INDEX_APP,
    COL_AU_APP,
    COL_AU_NPACK,
    COL_AU_TOTALLEN,
    COL_AU_F_NPACK,
    COL_AU_F_TOTALLEN,
    COL_AU_B_NPACK,
    COL_AU_B_TOTALLEN,
    NUM_AU_COLS
};

enum{
    COL_INDEX_SCAN,
    COL_FLOWID,
    COL_APP,
    COL_NPACK,
    COL_TOTALLEN,
    COL_MAXLEN,
    COL_MINLEN,
    COL_MEANLEN,
    COL_MAXIAT,
    COL_MINIAT,
    COL_TOTALIAT,
    COL_MEANIAT,
    COL_DURATION,
    COL_ACKCOUNT,
    COL_PSHCOUNT,
    COL_RSTCOUNT,
    COL_SYNCOUNT,
    COL_FINCOUNT,
    COL_TOTALWIN,
    COL_MAXWIN,
    COL_MINWIN,
    COL_MEANWIN,
    COL_F_NPACK,
    COL_F_TOTALLEN,
    COL_F_MAXLEN,
    COL_F_MINLEN,
    COL_F_MEANLEN,
    COL_F_MAXIAT,
    COL_F_MINIAT,
    COL_F_TOTALIAT,
    COL_F_MEANIAT,
    COL_F_DURATION,
    COL_F_ACKCOUNT,
    COL_F_PSHCOUNT,
    COL_F_RSTCOUNT,
    COL_F_SYNCOUNT,
    COL_F_FINCOUNT,
    COL_F_TOTALWIN,
    COL_F_MAXWIN,
    COL_F_MINWIN,
    COL_F_MEANWIN,
    COL_B_NPACK,
    COL_B_TOTALLEN,
    COL_B_MAXLEN,
    COL_B_MINLEN,
    COL_B_MEANLEN,
    COL_B_MAXIAT,
    COL_B_MINIAT,
    COL_B_TOTALIAT,
    COL_B_MEANIAT,
    COL_B_DURATION,
    COL_B_ACKCOUNT,
    COL_B_PSHCOUNT,
    COL_B_SYNCOUNT,
    COL_B_FINCOUNT,
    COL_B_TOTALWIN,
    COL_B_MAXWIN,
    COL_B_MINWIN,
    COL_B_MEANWIN,
    NUM_COLS
};

GObject *gtkScrolledWindow;

GtkWidget *view_Capture;
GtkTreeModel *model_Capture;
GtkListStore  *store_Capture;
GtkTreeIter iter_Capture;

GObject *gtkScrolledWindow2;

GtkWidget *view_AppUsage;
GtkTreeModel *model_AppUsage;
GtkListStore  *store_AppUsage;
GtkTreeIter iter_AppUsage;


GtkAdjustment * adj;

GObject *gtkToolBarStart;
GObject *gtkToolBarStop;
GObject *gtkToolBarRestart;
GObject *gtkToolBarStop;
GObject *gtkToolBarBlock;
GObject *gtkToolBarSave;
GObject *gtkToolBarClear;
GObject *gtkToolBarQuitMain;

int col_index = 0;
int app_index = 0;

pthread_t ptid_scan; 

void gtkStoreAppend(gchar *data){
    gdk_threads_init ();
    gdk_threads_enter ();
    
    char *FLOWID = strtok(data, "|");
    char *APP = strtok(NULL, "|");
    int NPACK = atoi(strtok(NULL, "|"));
    int TOTALLEN = atoi(strtok(NULL, "|"));
    int MAXLEN = atoi(strtok(NULL, "|"));
    int MINLEN = atoi(strtok(NULL, "|"));
    double MEANLEN = atof(strtok(NULL, "|"));
    double MAXIAT = atof(strtok(NULL, "|"));
    double MINIAT = atof(strtok(NULL, "|"));
    double TOTALIAT = atof(strtok(NULL, "|"));
    double MEANIAT = atof(strtok(NULL, "|"));
    double DURATION = atof(strtok(NULL, "|"));
    int ACKCOUNT = atoi(strtok(NULL, "|"));
    int PSHCOUNT = atoi(strtok(NULL, "|"));
    int RSTCOUNT = atoi(strtok(NULL, "|"));
    int SYNCOUNT = atoi(strtok(NULL, "|"));
    int FINCOUNT = atoi(strtok(NULL, "|"));
    int TOTALWIN = atoi(strtok(NULL, "|"));
    int MAXWIN = atoi(strtok(NULL, "|"));
    int MINWIN = atoi(strtok(NULL, "|"));
    double MEANWIN = atof(strtok(NULL, "|"));
    int F_NPACK = atoi(strtok(NULL, "|"));
    int F_TOTALLEN = atoi(strtok(NULL, "|"));
    int F_MAXLEN = atoi(strtok(NULL, "|"));
    int F_MINLEN = atoi(strtok(NULL, "|"));
    double F_MEANLEN = atof(strtok(NULL, "|"));
    double F_MAXIAT = atof(strtok(NULL, "|"));
    double F_MINIAT = atof(strtok(NULL, "|"));
    double F_TOTALIAT = atof(strtok(NULL, "|"));
    double F_MEANIAT = atof(strtok(NULL, "|"));
    double F_DURATION = atof(strtok(NULL, "|"));
    int F_ACKCOUNT = atoi(strtok(NULL, "|"));
    int F_PSHCOUNT = atoi(strtok(NULL, "|"));
    int F_RSTCOUNT = atoi(strtok(NULL, "|"));
    int F_SYNCOUNT = atoi(strtok(NULL, "|"));
    int F_FINCOUNT = atoi(strtok(NULL, "|"));
    int F_TOTALWIN = atoi(strtok(NULL, "|"));
    int F_MAXWIN = atoi(strtok(NULL, "|"));
    int F_MINWIN = atoi(strtok(NULL, "|"));
    double F_MEANWIN = atof(strtok(NULL, "|"));
    int B_NPACK = atoi(strtok(NULL, "|"));
    int B_TOTALLEN = atoi(strtok(NULL, "|"));
    int B_MAXLEN = atoi(strtok(NULL, "|"));
    int B_MINLEN = atoi(strtok(NULL, "|"));
    double B_MEANLEN = atof(strtok(NULL, "|"));
    double B_MAXIAT = atof(strtok(NULL, "|"));
    double B_MINIAT = atof(strtok(NULL, "|"));
    double B_TOTALIAT = atof(strtok(NULL, "|"));
    double B_MEANIAT = atof(strtok(NULL, "|"));
    double B_DURATION = atof(strtok(NULL, "|"));
    int B_ACKCOUNT = atoi(strtok(NULL, "|"));
    int B_PSHCOUNT = atoi(strtok(NULL, "|"));
    int B_SYNCOUNT = atoi(strtok(NULL, "|"));
    int B_FINCOUNT = atoi(strtok(NULL, "|"));
    int B_TOTALWIN = atoi(strtok(NULL, "|"));
    int B_MAXWIN = atoi(strtok(NULL, "|"));
    int B_MINWIN = atoi(strtok(NULL, "|"));
    double B_MEANWIN = atof(strtok(NULL, "|"));
    gtk_list_store_append (store_Capture , &iter_Capture);
    gtk_list_store_set (store_Capture, &iter_Capture,
            COL_INDEX_SCAN, col_index,
            COL_FLOWID, FLOWID,
            COL_APP, APP,
            COL_NPACK, NPACK,
            COL_TOTALLEN, TOTALLEN,
            COL_MAXLEN, MAXLEN,
            COL_MINLEN, MINLEN,
            COL_MEANLEN, MEANLEN,
            COL_MAXIAT, MAXIAT,
            COL_MINIAT, MINIAT,
            COL_TOTALIAT, TOTALIAT,
            COL_MEANIAT, MEANIAT,
            COL_DURATION, DURATION,
            COL_ACKCOUNT, ACKCOUNT,
            COL_PSHCOUNT, PSHCOUNT,
            COL_RSTCOUNT, RSTCOUNT,
            COL_SYNCOUNT, SYNCOUNT,
            COL_FINCOUNT, FINCOUNT,
            COL_TOTALWIN, TOTALWIN,
            COL_MAXWIN, MAXWIN,
            COL_MINWIN, MINWIN,
            COL_MEANWIN, MEANWIN,
            COL_F_NPACK, F_NPACK,
            COL_F_TOTALLEN, F_TOTALLEN,
            COL_F_MAXLEN, F_MAXLEN,
            COL_F_MINLEN, F_MINLEN,
            COL_F_MEANLEN, F_MEANLEN,
            COL_F_MAXIAT, F_MAXIAT,
            COL_F_MINIAT, F_MINIAT,
            COL_F_TOTALIAT, F_TOTALIAT,
            COL_F_MEANIAT, F_MEANIAT,
            COL_F_DURATION, F_DURATION,
            COL_F_ACKCOUNT, F_ACKCOUNT,
            COL_F_PSHCOUNT, F_PSHCOUNT,
            COL_F_RSTCOUNT, F_RSTCOUNT,
            COL_F_SYNCOUNT, F_SYNCOUNT,
            COL_F_FINCOUNT, F_FINCOUNT,
            COL_F_TOTALWIN, F_TOTALWIN,
            COL_F_MAXWIN, F_MAXWIN,
            COL_F_MINWIN, F_MINWIN,
            COL_F_MEANWIN, F_MEANWIN,
            COL_B_NPACK, B_NPACK,
            COL_B_TOTALLEN, B_TOTALLEN,
            COL_B_MAXLEN, B_MAXLEN,
            COL_B_MINLEN, B_MINLEN,
            COL_B_MEANLEN, B_MEANLEN,
            COL_B_MAXIAT, B_MAXIAT,
            COL_B_MINIAT, B_MINIAT,
            COL_B_TOTALIAT, B_TOTALIAT,
            COL_B_MEANIAT, B_MEANIAT,
            COL_B_DURATION, B_DURATION,
            COL_B_ACKCOUNT, B_ACKCOUNT,
            COL_B_PSHCOUNT, B_PSHCOUNT,
            COL_B_SYNCOUNT, B_SYNCOUNT,
            COL_B_FINCOUNT, B_FINCOUNT,
            COL_B_TOTALWIN, B_TOTALWIN,
            COL_B_MAXWIN, B_MAXWIN,
            COL_B_MINWIN, B_MINWIN,
            COL_B_MEANWIN, B_MEANWIN,
            -1);
    
    adj = gtk_scrolled_window_get_vadjustment (gtkScrolledWindow);
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
    col_index++;
    
    //AppUsage
    gboolean valid;
    gboolean done;

    gchar *app_name;
    gint *pack;
    gint *totlen;
    gint *fpack;
    gint *ftotlen;
    gint *bpack;
    gint *btotlen;

    done = FALSE;
    
    valid = gtk_tree_model_get_iter_first (store_AppUsage,
                                       &iter_AppUsage);
    
    while (valid)
    {

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                            COL_AU_APP, &app_name,
                            -1);


    if(strcmp(app_name, APP) == 0){

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_NPACK, &pack,
                      -1);

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_TOTALLEN, &totlen,
                      -1);

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_F_NPACK, &fpack,
                      -1);

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_F_TOTALLEN, &ftotlen,
                      -1);

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_B_NPACK, &bpack,
                      -1);\

        gtk_tree_model_get (model_AppUsage, &iter_AppUsage,
                      COL_AU_B_TOTALLEN, &btotlen,
                      -1);                                                                      

        gtk_list_store_set(store_AppUsage,&iter_AppUsage,
                            2,(NPACK + pack),
                            3,(TOTALLEN + totlen),
                            4,(F_NPACK + fpack),
                            5,(F_TOTALLEN + ftotlen),
                            6,(B_NPACK + bpack),
                            7,(B_TOTALLEN + btotlen),
                            -1);

        done = TRUE;
        break;                  
      }

    valid = gtk_tree_model_iter_next (store_AppUsage,
                        &iter_AppUsage);
    
    }

    if(!(done)){
        gtk_list_store_append (store_AppUsage , &iter_AppUsage);
        gtk_list_store_set (store_AppUsage, &iter_AppUsage,
            COL_AU_INDEX_APP, app_index,
            COL_AU_APP, APP,
            COL_AU_NPACK, NPACK,
            COL_AU_TOTALLEN, TOTALLEN,
            COL_AU_F_NPACK, F_NPACK,
            COL_AU_F_TOTALLEN, F_TOTALLEN,
            COL_AU_B_NPACK, B_NPACK,
            COL_AU_B_TOTALLEN, B_TOTALLEN,
            -1);

        app_index++;
    }
     /*   
      g_free (app_name);
      g_free (pack);
      g_free (totlen);
      g_free (fpack);
      g_free (ftotlen);
      g_free (bpack);
      g_free (btotlen);
      */
    gdk_threads_leave ();
}

static GtkTreeModel * create_and_fill_model_scan (void){
    
    store_Capture = gtk_list_store_new (NUM_COLS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_DOUBLE,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_DOUBLE);

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

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total len",  
                                                renderer,
                                                "text", COL_TOTALLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max len",  
                                                renderer,
                                                "text", COL_MAXLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Min len",  
                                                renderer,
                                                "text", COL_MINLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean len",  
                                                renderer,
                                                "text", COL_MEANLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max IAT",  
                                                renderer,
                                                "text", COL_MAXIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean IAT",  
                                                renderer,
                                                "text", COL_MINIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total IAT",  
                                                renderer,
                                                "text", COL_TOTALIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean IAT",  
                                                renderer,
                                                "text", COL_MEANIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Duration",  
                                                renderer,
                                                "text", COL_DURATION,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Ack count",  
                                                renderer,
                                                "text", COL_ACKCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Psh count",  
                                                renderer,
                                                "text", COL_PSHCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Rst count",  
                                                renderer,
                                                "text", COL_RSTCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Syn count",  
                                                renderer,
                                                "text", COL_SYNCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Fin count",  
                                                renderer,
                                                "text", COL_FINCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Total win",  
                                                renderer,
                                                "text", COL_TOTALWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Max win",  
                                                renderer,
                                                "text", COL_MAXWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Min win",  
                                                renderer,
                                                "text", COL_MINWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Mean win",  
                                                renderer,
                                                "text", COL_MEANWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° F Pkts",  
                                                renderer,
                                                "text", COL_F_NPACK,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total len",  
                                                renderer,
                                                "text", COL_F_TOTALLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max len",  
                                                renderer,
                                                "text", COL_F_MAXLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Min len",  
                                                renderer,
                                                "text", COL_F_MINLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean len",  
                                                renderer,
                                                "text", COL_F_MEANLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max IAT",  
                                                renderer,
                                                "text", COL_F_MAXIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean IAT",  
                                                renderer,
                                                "text", COL_F_MINIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total IAT",  
                                                renderer,
                                                "text", COL_F_TOTALIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean IAT",  
                                                renderer,
                                                "text", COL_F_MEANIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Duration",  
                                                renderer,
                                                "text", COL_F_DURATION,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Ack count",  
                                                renderer,
                                                "text", COL_F_ACKCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Psh count",  
                                                renderer,
                                                "text", COL_F_PSHCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Rest count",  
                                                renderer,
                                                "text", COL_F_RSTCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Syn Count",  
                                                renderer,
                                                "text", COL_F_SYNCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Fin count",  
                                                renderer,
                                                "text", COL_F_FINCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Total win",  
                                                renderer,
                                                "text", COL_F_TOTALWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Max win",  
                                                renderer,
                                                "text", COL_F_MAXWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Min win",  
                                                renderer,
                                                "text", COL_F_MINWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F Mean win",  
                                                renderer,
                                                "text", COL_F_MEANWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N° B Pkts",  
                                                renderer,
                                                "text", COL_B_NPACK,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total len",  
                                                renderer,
                                                "text", COL_B_TOTALLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max len",  
                                                renderer,
                                                "text", COL_B_MAXLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Min len",  
                                                renderer,
                                                "text", COL_B_MINLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean len",  
                                                renderer,
                                                "text", COL_B_MEANLEN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max IAT",  
                                                renderer,
                                                "text", COL_B_MAXIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean IAT",  
                                                renderer,
                                                "text", COL_B_MINIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total IAT",  
                                                renderer,
                                                "text", COL_B_TOTALIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean IAT",  
                                                renderer,
                                                "text", COL_B_MEANIAT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Duration",  
                                                renderer,
                                                "text", COL_B_DURATION,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Ack count",  
                                                renderer,
                                                "text", COL_B_ACKCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Psh count",  
                                                renderer,
                                                "text", COL_B_PSHCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Syn count",  
                                                renderer,
                                                "text", COL_B_SYNCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Fin count",  
                                                renderer,
                                                "text", COL_B_FINCOUNT,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Total win",  
                                                renderer,
                                                "text", COL_B_TOTALWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Max win",  
                                                renderer,
                                                "text", COL_B_MAXWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B min win",  
                                                renderer,
                                                "text", COL_B_MINWIN,
                                                NULL);
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B Mean win",  
                                                renderer,
                                                "text", COL_B_MEANWIN,
                                                NULL);


    model_Capture = create_and_fill_model_scan ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model_Capture);

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model_Capture);

    return view;
};

static GtkTreeModel * create_and_fill_model_app (void){
    
    store_AppUsage = gtk_list_store_new (NUM_AU_COLS,
                                    G_TYPE_INT,
                                    G_TYPE_STRING,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT,
                                    G_TYPE_INT);

    return GTK_TREE_MODEL (store_AppUsage);
};

static GtkWidget * create_view_and_model_app (void){
    
    GtkCellRenderer     *renderer;
    GtkWidget           *view;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "N°",  
                                                renderer,
                                                "text", COL_AU_INDEX_APP,
                                                NULL);
    
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Application",  
                                                renderer,
                                                "text", COL_AU_APP,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Packs",  
                                                renderer,
                                                "text", COL_AU_NPACK,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "Totallen",  
                                                renderer,
                                                "text", COL_AU_TOTALLEN,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F_packs",  
                                                renderer,
                                                "text", COL_AU_F_NPACK,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "F_Totallen",  
                                                renderer,
                                                "text", COL_AU_F_TOTALLEN,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B_packs",  
                                                renderer,
                                                "text", COL_AU_B_NPACK,
                                                NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                -1,      
                                                "B_Totallen",  
                                                renderer,
                                                "text", COL_AU_B_TOTALLEN,
                                                NULL);                                                                                                                                                                                

    model_AppUsage = create_and_fill_model_app();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model_AppUsage);

    /* The tree view has acquired its own reference to the
    *  model, so we can drop ours. That way the model will
    *  be freed automatically when the tree view is destroyed */

    g_object_unref (model_AppUsage);

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
    gtk_list_store_clear (store_AppUsage);

    col_index = 0;
    app_index = 0;

    gtkToolBarStop_clicked(gtkToolBarStop, data);
    gtkToolBarStart_clicked(gtkToolBarStart, data);
    

}
void gtkToolBarBlock_clicked(GtkWidget *widget, gpointer data) {}
void gtkToolBarSave_clicked(GtkWidget *widget, gpointer data) {}
void gtkToolBarClear_clicked(GtkWidget *widget, gpointer data) {
    gtk_list_store_clear (store_Capture);
    gtk_list_store_clear (store_AppUsage);

    col_index = 0;
    app_index = 0;
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
    gtkToolBarRestart = gtk_builder_get_object(builder, "gtkToolBarRestart");
    gtkToolBarStop = gtk_builder_get_object(builder, "gtkToolBarStop");
    gtkToolBarBlock = gtk_builder_get_object(builder, "gtkToolBarBlock");
    gtkToolBarSave = gtk_builder_get_object(builder, "gtkToolBarSave");
    gtkToolBarClear = gtk_builder_get_object(builder, "gtkToolBarClear");
    gtkToolBarQuitMain = gtk_builder_get_object(builder, "gtkToolBarQuitMain");


    gtkScrolledWindow = gtk_builder_get_object(builder,"gtkScrolledWindow");
    gtkScrolledWindow2 = gtk_builder_get_object(builder,"gtkScrolledWindow2");

    
    
    view_Capture =  create_view_and_model_scan ();
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow), view_Capture);
    gtk_widget_set_sensitive(gtkToolBarStop, FALSE);


    //AppUsage 
    view_AppUsage =  create_view_and_model_app ();
    gtk_container_add (GTK_CONTAINER (gtkScrolledWindow2), view_AppUsage);
    gtk_widget_set_sensitive(gtkToolBarStop, FALSE);


    g_signal_connect (gtkToolBarStart, "clicked", gtkToolBarStart_clicked, NULL);
    g_signal_connect (gtkToolBarStop, "clicked", gtkToolBarStop_clicked, NULL);
    g_signal_connect (gtkToolBarRestart, "clicked", gtkToolBarRestart_clicked, NULL);
    g_signal_connect (gtkToolBarBlock, "clicked", gtkToolBarBlock_clicked, NULL);
    g_signal_connect (gtkToolBarSave, "clicked", gtkToolBarSave_clicked, NULL);
    g_signal_connect (gtkToolBarClear, "clicked", gtkToolBarClear_clicked, NULL);
    g_signal_connect (gtkToolBarQuitMain, "clicked", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (gtkWindow, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    

    gtk_widget_show_all(gtkWindow);

    
    gtk_main ();

    return 0;
}
