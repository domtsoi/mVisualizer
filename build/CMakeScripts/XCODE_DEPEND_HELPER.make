# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.objMorphVisualizer.Debug:
PostBuild.glfw.Debug: /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Debug/objMorphVisualizer
/Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Debug/objMorphVisualizer:\
	/usr/local/Cellar/glfw-3.2.1/debug/src/Debug/libglfw3.a
	/bin/rm -f /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Debug/objMorphVisualizer


PostBuild.glfw.Debug:
/usr/local/Cellar/glfw-3.2.1/debug/src/Debug/libglfw3.a:
	/bin/rm -f /usr/local/Cellar/glfw-3.2.1/debug/src/Debug/libglfw3.a


PostBuild.objMorphVisualizer.Release:
PostBuild.glfw.Release: /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Release/objMorphVisualizer
/Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Release/objMorphVisualizer:\
	/usr/local/Cellar/glfw-3.2.1/debug/src/Release/libglfw3.a
	/bin/rm -f /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/Release/objMorphVisualizer


PostBuild.glfw.Release:
/usr/local/Cellar/glfw-3.2.1/debug/src/Release/libglfw3.a:
	/bin/rm -f /usr/local/Cellar/glfw-3.2.1/debug/src/Release/libglfw3.a


PostBuild.objMorphVisualizer.MinSizeRel:
PostBuild.glfw.MinSizeRel: /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/MinSizeRel/objMorphVisualizer
/Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/MinSizeRel/objMorphVisualizer:\
	/usr/local/Cellar/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a
	/bin/rm -f /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/MinSizeRel/objMorphVisualizer


PostBuild.glfw.MinSizeRel:
/usr/local/Cellar/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a:
	/bin/rm -f /usr/local/Cellar/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a


PostBuild.objMorphVisualizer.RelWithDebInfo:
PostBuild.glfw.RelWithDebInfo: /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/RelWithDebInfo/objMorphVisualizer
/Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/RelWithDebInfo/objMorphVisualizer:\
	/usr/local/Cellar/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a
	/bin/rm -f /Users/dominictsoi/Desktop/CPE\ _400_Music_Visualizer/objMorphVisualizer/build/RelWithDebInfo/objMorphVisualizer


PostBuild.glfw.RelWithDebInfo:
/usr/local/Cellar/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a:
	/bin/rm -f /usr/local/Cellar/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a




# For each target create a dummy ruleso the target does not have to exist
/usr/local/Cellar/glfw-3.2.1/debug/src/Debug/libglfw3.a:
/usr/local/Cellar/glfw-3.2.1/debug/src/MinSizeRel/libglfw3.a:
/usr/local/Cellar/glfw-3.2.1/debug/src/RelWithDebInfo/libglfw3.a:
/usr/local/Cellar/glfw-3.2.1/debug/src/Release/libglfw3.a:
