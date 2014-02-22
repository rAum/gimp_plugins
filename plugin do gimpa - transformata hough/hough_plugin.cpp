/********************************************************************
 *                                  (c) Andrzej Lukaszewski 2009-2011
 * Gimp Plugin :
 *             Simple plugin template without dialog only message box
 * 
 * This file: gimp plugin interface 
 ********************************************************************/
#include <libgimp/gimp.h>


void cpu_process(unsigned char *buffer,int width,int height,int channels);


static void query(void)
{
   static GimpParamDef args[]={
      {GIMP_PDB_INT32   ,"run-mode","Run mode"      },
      {GIMP_PDB_IMAGE   ,"image"   ,"Input image"   },
      {GIMP_PDB_DRAWABLE,"drawable","Input drawable"}
   };

   gimp_install_procedure(
      "plug-in-hough",
      "plug-in-test2",
      "plug-in-test3",
      "anl",
      "Copyright MF",
      "2011",
      "Transformata Hough",
      "RGB",
      GIMP_PLUGIN,
      G_N_ELEMENTS (args), 0,
      args, NULL
   );

   gimp_plugin_menu_register("plug-in-hough","<Image>/Filters/Misc");
}


static void render(GimpDrawable *drawable)
{
   gint         channels;
   gint         x1,y1,x2,y2;
   GimpPixelRgn src,dst;
   guchar       *buffer;
   gint         width,height;

   gimp_drawable_mask_bounds(drawable->drawable_id,&x1,&y1,&x2,&y2);
   channels=gimp_drawable_bpp(drawable->drawable_id);

   width =x2-x1;
   height=y2-y1;

   gimp_pixel_rgn_init(&src,drawable,x1,y1,width,height,FALSE,FALSE);
   gimp_pixel_rgn_init(&dst,drawable,x1,y1,width,height,TRUE ,TRUE );

   buffer=g_new(guchar,channels*width*height);

   gimp_pixel_rgn_get_rect(&src,buffer,x1,y1,width,height);

   // Here it goes ;)
   cpu_process(buffer,width,height,channels);

   gimp_pixel_rgn_set_rect(&dst,buffer,x1,y1,width,height);

   g_free(buffer);

   gimp_drawable_flush(drawable);
   gimp_drawable_merge_shadow(drawable->drawable_id,TRUE);
   gimp_drawable_update(drawable->drawable_id,x1,y1,width,height);
}


static void run(const gchar *name,gint nparams,const GimpParam *param,gint *nreturn_vals,GimpParam **return_vals)
{
   static GimpParam  values[1];
   GimpPDBStatusType status   = GIMP_PDB_SUCCESS;
   GimpRunMode       run_mode = (GimpRunMode)param[0].data.d_int32;
   GimpDrawable      *drawable;

   // Setting mandatory output values
   *nreturn_vals=1;
   *return_vals =values;

   values[0].type          = GIMP_PDB_STATUS;
   values[0].data.d_status = status;

   // Get specified drawable
   drawable=gimp_drawable_get(param[2].data.d_drawable);

   render(drawable);

   if (run_mode!=GIMP_RUN_NONINTERACTIVE) {
      g_message("CPU Filtering 'anl test' done !\n");
   }

   gimp_displays_flush();
   gimp_drawable_detach(drawable);
}


GimpPlugInInfo PLUG_IN_INFO={
   NULL,
   NULL,
   query,
   run
};

MAIN()
