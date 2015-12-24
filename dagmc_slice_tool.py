
import numpy as np
from matplotlib.path import Path
from matplotlib.patches import PathPatch 
import matplotlib.pyplot as plt
import numpy as np
from dag_slicer.dag_slicer import Dag_Slicer 
from matplotlib.widgets import CheckButtons, RadioButtons


class dagmc_slicer(Dag_Slicer):

    #wrapper for the super init 
    def __init__(self, filename, axis = 0, coordinate = 0, by_group = False):
        
        super(dagmc_slicer, self).__init__( filename, axis, coordinate, by_group )
        self.shown = False
        self.color_seed = 56
        
    def clear_slice(self):
        #clear old arrays so there isn't junk data in the way
        self.slice_x_pnts = np.array([])
        self.slice_y_pnts = np.array([])
        self.path_coding = np.array([], dtype='int')
        self.group_names = np.array([], dtype='str')
        self.shown = False 

    def create_slice(self):
        
        #clear out old info
        self.clear_slice()

        #clear old arrays so there isn't junk data in the way
        self.slice_x_pnts = np.array([])
        self.slice_y_pnts = np.array([])
        self.path_coding = np.array([], dtype='int')
        self.group_names = np.array([], dtype='str')

        #run the super function to create the slice
        super(dagmc_slicer, self).create_slice()
            
    def rename_group(self, id, new_name):
        super(dagmc_slicer, self).rename_group(id, new_name)

    def show_slice(self, colors=None):        

        if 0 == len(self.slice_x_pnts):
            self.create_slice()


        #now setup the plot object
        all_paths = []
        for i in range(len(self.slice_x_pnts)):
            new_list = [ np.transpose(np.vstack((self.slice_x_pnts[i],self.slice_y_pnts[i]))), self.path_coding[i]]
            all_paths.append(new_list)

        if colors == None:
            colors = []
            np.random.seed(self.color_seed)
            for i in range(len(all_paths)):
                colors.append(np.random.rand(3,).tolist())
        elif len(colors) != len(all_paths):
            raise ValueError("{} colors are required, {} colors have been specified".format(
                             len(colors), len(all_paths)))

        #create the patches for this plot
        patches = []
        for i, (coord, code) in enumerate(all_paths):
            path = Path(coord, code)
            patches.append(PathPatch(path, color=colors[i], ec='black', lw=1))

                  
        #create a new figure
        fig, ax = plt.subplots()
        self.figure = fig
        self.plt_ax = ax
        
        
        #add the patches to the plot
        for patch in patches:
            ax.add_patch(patch)

        if 0 != len(self.group_names):
            leg = ax.legend(patches, self.group_names, prop={'size':14}, loc=2, bbox_to_anchor=(1.05,1.), borderaxespad=0.)
            #create mapping of artist to legend entry
            self.legend_map = {}
            for legpatch, patch in zip(leg.get_patches(), patches):
                legpatch.set_picker(True)
                self.legend_map[legpatch] = patch

                #setup the check boxex
                cax = plt.axes([0.025, 0.5, 0.12, 0.12])
                self.check = CheckButtons( cax, ('Visible','Filled'),(True,True) )
                self.check.visible = False
                self.check.on_clicked(self.visiblefunc)


        #plot axis settings
        ax.autoscale_view()
        ax.set_aspect('equal')


        cid = self.figure.canvas.mpl_connect('pick_event', self.onpick)

        plt.show()
        self.shown = True


    def onpick(self,event):
        self.picked = event.artist
        #Reset all legend items to black then highlight current selection
        [a.set_edgecolor('black') for a in self.legend_map.keys()]
        event.artist.set_edgecolor('orange')

        #Get the patch item through the legend map and update the checkbox settings
        origpatch = self.legend_map[event.artist]
        [l.set_visible( origpatch.get_visible() ) for l in self.check.lines[0]]
        [l.set_visible( origpatch.get_fill() ) for l in self.check.lines[1]]
        #Redraw the plot
        self.figure.canvas.draw()

    def visiblefunc(self,label):
        #Check the current visibility/fill of the patch based
        #on the state of the check boxes
        vis = self.check.lines[0][0].get_visible()
        filled = self.check.lines[1][0].get_visible()
        #Reflect the changes to the patch in the legend item
        self.picked.set_alpha( 1.0 if vis else 0.6 )        
        self.picked.set_fill(filled)
        #Make changes to the original patch
        origpatch = self.legend_map[self.picked]
        origpatch.set_visible(vis)
        origpatch.set_fill(filled)
        #Redraw the plot
        self.figure.canvas.draw()
        
