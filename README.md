FluoRender
========

FluoRender Source Code

This is the open-source repository for FluoRender, an interactive rendering tool for confocal microscopy data visualization. It combines the renderings of multi-channel volume data and polygon mesh data, where the properties of each dataset can be adjusted independently and quickly. The tool is designed especially for neurobiologists, and it helps them better visualize the fluorescent-stained confocal samples.

Aknowledgements
========
If you use FluoRender in work that leads to published research, we humbly ask that you add the following to the 'Acknowledgments' section of your paper: 
"This work was made possible in part by software funded by the NIH: Fluorender: Visualization-Based and Interactive Analysis for Multichannel Microscopy Data, R01EB023947."
If you would like to cite FluoRender, you may reference the following publication:
Wan, Y., et al. (2017). FluoRender: joint free-hand segmentation and visualization for many-channel fluorescence data analysis. BMC Bioinformatics, 18:280.

<strong>Author: </strong> Yong Wan<br/>
<strong>Developer: </strong> Brig Bagley<br/>

Requirements
========
 * Git (https://git-scm.com/)
 * CMake 2.6+ (http://www.cmake.org/)
 * wxWidgets (https://github.com/wxWidgets/wxWidgets)
 * Windows 7+ : Visual Studio 10.0 2010+ (also tested on Windows 10 / MSVC 14.0 2015)
 * OSX 10.9+  : Latest Xcode and command line tools, homebrew
 * Other platforms may work, but are not officially supported.
 * Boost 1.55.0+ (http://www.boost.org/users/download/#live)


Building FluoRender
========
We recommend building FluoRender outside of the source tree. <br/>
<h4>OSX</h4> 

1) Clone the latest wxWidgets using GIT (<code>git clone git@github.com:wxWidgets/wxWidgets.git</code>).
   
   * The steps following will assume the wxWidgets root directory is at <code>~/wxWidgets</code>

2) Build wxWidgets from the command line.
   * <code>cd ~/wxWidgets/</code>
   
   * <code>mkdir mybuild</code>
   
   * <code>cd mybuild</code>
   
   * <code>../configure --disable-shared --enable-macosx_arch=x86_64 --with-cocoa --with-macosx-version-min=10.9 --enable-cxx11 --with-cxx=11 --enable-stl --enable-std_containers --enable-std_iostreams --enable-std_string --enable-std_string_conv_in_wxstring --with-libpng=builtin --with-libtiff=builtin --with-libjpeg=builtin</code>
   
   * <code>make</code>

3) Download and build boost.

   * Download boost (http://www.boost.org/users/download/#live) and extract onto your machine.
   
   * Build boost using <code>./bootstrap.sh</code> and <code>./b2</code> in the boost directory.
   
   * The steps following will assume the boost root directory is at <code>~/boost_1_55_0</code> (your version might differ).

4) Get homebrew, libtiff, and freetype

   * <code>ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"</code>
   
   * <code>brew install libtiff</code>

   * <code>brew install freetype</code>

5) Get and build FluoRender

   * <code>git clone git@github.com:SCIInstitute/fluorender.git</code><br/>
   
   * <code>cd fluorender</code><br/>
   
   * <code>mkdir build</code><br/>
   
   * <code>cd build</code><br/>

   * <code>cmake -G Xcode -DwxWidgets_CONFIG_EXECUTABLE="~/wxWidgets/mybuild/wx-config" -DwxWidgets_wxrc_EXECUTABLE="~/wxWidgets/mybuild/utils/wxrc/wxrc" -DwxWidgets_USE_DEBUG=ON -DwxWidgets_ROOT_DIR="~/wxWidgets" -DBoost_INCLUDE_DIR="/Users/YourUserName/boost_1_55_0" -DCMAKE_BUILD_TYPE="Debug" ..</code> (replace directories with your versions)

5) Open the Xcode file generated to build and run FluoRender.

<h4>Windows</h4> 

1) Clone the latest wxWidgets using GIT (<code>git clone git@github.com:wxWidgets/wxWidgets.git</code>).
   
   * The steps following will assume the wxWidgets repository is at <code>C:\wxWidgets</code>

2) Open a 64 bit Visual Studio command prompt to build wxWidgets. (make sure you use the prompt version you wish to build all dependencies, IE , MSVC 14.0 2015 x64)

   * Go to directory <code>C:\wxWidgets\build\msw</code>
  
   * Type <code>nmake /f makefile.vc TARGET_CPU=x64 BUILD=debug</code> to build debug libraries.

   * Type <code>nmake /f makefile.vc TARGET_CPU=x64 BUILD=release</code> to build release libraries.
   
3) Download and build boost.

   * Download boost (http://www.boost.org/users/download/#live) and extract onto your machine.
   
   * Build boost using <code>bootstrap.exe</code> and <code>b2.exe --toolset=msvc-14.0 --build-type=complete architecture=x86 address-model=64 stage</code> in the boost directory in a MSVC prompt. (change the toolset to the version of MSVC you are using, and omit address-model and architecture for 32-bit)
   
   * The steps following will assume the boost root directory is at <code>C:\boost_1_55_0</code> (your version might differ).

4) You may need to add lines to <code>C:\Program Files (x86)\CMake X.X\share\cmake-x.x\Modules\FindwxWidgets.cmake</code> (x's are your version) for wxWidgets 3.* if it still complains that you haven't installed wxWidgets.
   
   * Starting about line 277, you will have listed a few sets of library versions to search for like <code>wxbase29${_UCD}${_DBG}</code> <br/>
   
   * In 4 places, you will need to add above each line with a "29" a new line that is exactly the same, but with a "31" instead, assuming your version of wxWidgets is 3.1.*). <br/>

5) Download FluoRender using Git <code>git clone git@github.com:SCIInstitute/fluorender.git</code>

6) Use the <code>C:\Program Files(x86)\CMake2.8\bin\cmake-gui.exe</code> program to configure build properties and generate your Visual Studio Solution file. (Remember to keep your MSVC version consistent)
   
   * Select your FluoRender source and build directories (create a new folder for building), and add the locations of boost and wxWidgets. <br/>
   	- Choose the FluoRender main folder for source and create a new folder for the build. <br/>
   	
   	- Click Configure.  NOTE: You may need to display advanced options to set below options. <br/>
   	
   	- Choose the build type <code>CMAKE_BUILD_TYPE</code> to be "Debug" or "Release" <br/>

   	- Be sure to set <code>wxWidgets_LIB_DIR</code> to <code>C:\wxWidgets\lib\vc_x64_lib</code>. (this will differ from 32 bit)
   	
   	- Be sure to set <code>wxWidgets_ROOT_DIR</code> to <code>C:\wxWidgets</code>.
   	
   	- Be sure to set <code>Boost_INCLUDE_DIR</code> to <code>C:\boost_1_55_0</code> (assuming this is your boost dir). <br/>
   	
   	- Click Generate. 

   * You may also generate using the command prompt, but you must explicitly type the paths for the cmake command. <br/>
   
    - Open Visual Studio Command Prompt. Go to the CMakeLists.txt directory. <br/>
    	
    - Type <code> cmake -G "Visual Studio 14 2015 Win64" -DwxWidgets_LIB_DIR="C:\wxWidgets\lib\vc_x64_lib" -DwxWidgets_ROOT_DIR="C:\wxWidgets" -DBoost_INCLUDE_DIR="C:\boost_1_55_0" -DCMAKE_BUILD_TYPE="Debug" ..</code> in your build directory (again assuming these are your directory locations / Generator versions, and the build folder is in the FluoRender root directory). <br/>
    	
   * Open the Visual Studio SLN file generated by CMake (found in your "build" directory). <br/>
   
   * Build the solution. Use CMake to generate both "Release" and "Debug" configurations if you wish to build both in Visual Studio.<br/><br/>
    	**Notes for Visual Studio**
    - Visual Studio may not set the correct machine target when building 64 bit. 
     Check <code>Project Properties -> Configuration Properties -> Linker -> Command line</code>. Make sure "Additional Options" is <code>/machine:X64</code> NOT <code>/machine:X86</code>. <br/>
    	
    - You may need to right-click FluoRender project on the Solution Explorer to "Set as StartUp Project" for it to run. <br/>
    - If you are building on Windows 8 or later, you will need to set a Visual Studio Graphics Option. This enables the application to build in higher definition.<br/>
      <code>Project Properties -> Manifest Tool -> Input and Output -> Enable DPI Awareness -> Yes </code> <br/>

<h4>Linux (Ubuntu)</h4> 

1) Make sure your enviornment is set up
  * g++ <code>sudo apt install g++</code>
  * Package Config <code>sudo apt install pkg-config</code>
  * Yasm <code>sudo apt install yasm</code>
  * git <code>sudo apt install git</code>
  * libgtk <code>sudo apt install libgtk-3-dev</code>
  * OpenCL <code>sudo apt install ocl-icd-libopencl1</code>
  * OpenGL <code>sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev</code>
  * libtiff <code>sudo apt install libtiff</code>
  * libglib <code>sudo apt install libglib2.0-0</code>

2) Install CMake
  * Download the latest version of CMake from: https://cmake.org/download/ (This guide used 3.14.0-rc2)
    - You can either use the archive manager or extract the tar.gz file yourself <br/>
  * Go to the location of your tar file with your command line
  * run <code>./bootstrap</code>
  * run <code>make</code> (depending on how many cores you have on your computer, you can run make -j4, where 4 is four cores.)
  * run <code>sudo make install</code> when make is finished.
  * To verify installation, type <code>cmake --version</code>

3) Install Boost
  * Download the latest version of Boost from: https://www.boost.org/users/download/ (This guide used 1.69)
    - You can either use the archive manager or extract the tar.gz file yourself <br/>
  * Go to the location of your tar file with your command line
  * run <code>./bootstrap.sh</code>
  * run <code>sudo ./b2 install</code>

4) Install Java
  * Download the latest JDK from: https://www.oracle.com/technetwork/java/javase/downloads/index.html (This guide used 11.0.2 LTS)
    - You must accept the license agreement and download the tar.gz file.<br/>
  * Create a directory in your <code>/usr/local/</code> if you have permissions and name it <code>java</code>.
    - <code>sudo mkdir java</code> (the path should be /usr/local/java/)<br>
  * In your command line, navigate to the directory where you downloaded the tar file.
  * run <code>sudo tar -zxf [name of tar file, mine was jdk-11.0.2_linux-x64_bin.tar.gz] -C /usr/local/java</code>
  * Update the alternatives:
    - <code>sudo update-alternatives --install /usr/bin/java java /usr/local/java/[your version of the jdk, mine was jdk-11.0.2]/bin/java 1500</code><br/>
    - <code>sudo update-alternatives --install /usr/bin/javac javac /usr/local/java/[your version of the jdk, mine was jdk-11.0.2]/bin/javac 1500</code><br/>
  * Set the JAVA_HOME enviornment variable
    - run <code>sudo vim (or your preferred text editor) /etc/environment</code><br/>
    - create a new line and type <code>JAVA_HOME="/usr/local/java/[your version of the jdk, mine was jdk-11.0.2]</code><br/>
    - save and close the file.<br/>
    - run <code>source /etc/enviornment</code> or reboot your system.
    - run <code> echo $JAVA_HOME$</code> to verify your home path.
  * To verify installation, run <code> java -version</code>

5) Install CUDA
  * This may only be necessary with NVIDIA GPU's, I am not sure of this step, I installed incase of OpenCL/GL Dependencies
  * Download the latest version of CUDA at: https://developer.nvidia.com/cuda-downloads
    - Select Linux<br/>
    - Select x86_64<br/>
    - Select Ubuntu<br/>
    - Select Distro version<br/>
    - Select runfile(local), note: If you wish to build this manually, you can, I found it easier to run the run file.<br/>
  * In your command line, navigate to the download and run <code>sudo sh [your download filename]</code>
  * Follow directions in command line and reboot computer.

6) Install wxWidgets
  * Download the latest version of wxWidgets from: https://www.wxwidgets.org/downloads/ (This guide used 3.1.2)
    - You can either use the archive manager or extract the tar.gz file yourself <br/>
  * In your command line, navigate to the wXWidgets folder and run:
    - <code>./configure --prefix=/uufs/chpc.utah.edu/sys/installdir/wxWidgets/3.0.4-5.4.0g --with-libpng --with-libjpeg --with-libtiff --with-libxpm --with-libiconv --with-libnotify --with-opengl --with-regex --with-zlib --enable-cxx11 --enable-stl --enable-monolithic</code><br/>
  * run <code>make</code> (depending on how many cores you have on your computer, you can run make -j4, where 4 is four cores.)
  * run <code>sudo make install</code>
  * navigate to your install, mine was located at <code>/usr/local/include</code>
  * run <code>sudo ln -sv [your wx widgets folder, mine was wx-3.1] wx</code>

7) Build the Project
  * If everything was installed correctly this should run with no issues.
  * clone the project from: https://github.com/SCIInstitute/fluorender.git
  * navigate to the project folder and create a directory called build.
  * create a new directory in <code>fluorender/ffmpeg/lib/</code> and name it Unix.
  * extract the contents of fluoLibUnix.tar.gz into <code>fluorender/ffmpeg/lib/Unix</code>
  * from the root of fluorender, run <code>cmake ..</code>
  * run make

Contact
========

If there are any problems, email: fluorender@sci.utah.edu
