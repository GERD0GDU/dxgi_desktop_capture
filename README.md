DXGI Desktop Capture
===============================

Project that captures the desktop image with DXGI duplication. Saves the captured image to the file in different image formats (*.bmp; *.jpg; *.tif).

This application can be interesting, because it presents a solution with drawing of current icon of cursor into the captured desktop screen image. (Pixel Alpha Blending)

**Warning**  This sample requires Microsoft Visual Studio 2013 or a later version (any SKU) and will not compile in Microsoft Visual Studio Express 2013 for Windows.

This sample is written in C++. You also need some experience with DirectX (D3D11, D2D1).

To get a copy of Windows, go to [Downloads and tools](http://go.microsoft.com/fwlink/p/?linkid=301696).

To get a copy of Visual Studio, go to [Visual Studio Downloads](http://go.microsoft.com/fwlink/p/?linkid=301697).

Operating system requirements
-----------------------------

Client

Windows 8.1

Server

Windows Server 2012 R2

What the Application Can Do
---------------------------

- If you have more than one desktop monitor, you can choose.
- **Resize** the captured desktop image.
- Choose different **scaling modes**. (It is like the [SizeMode](https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.picturebox.sizemode?view=netcore-3.1#System_Windows_Forms_PictureBox_SizeMode) property of [System.Windows.Forms.PictureBox](https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.picturebox?view=netcore-3.1).)
  - **Normal**: The image is placed in the upper-left corner of the output picture. The image is clipped if it is larger than the output picture size it is contained in.
  - **StretchImage**: The image within the output picture is stretched or shrunk to fit the size of the output picture size.
  - **AutoSize**: The output picture is sized equal to the size of the image that it contains.
  - **CenterImage**: The image is displayed in the center if the output picture is larger than the image. If the image is larger than the output picture size, the picture is placed in the center of the output picture and the outside edges are clipped.
  - **Zoom**: The size of the image is increased or decreased maintaining the size ratio. Empty areas of the output image are filled in black.
- You can **show or hide** the mouse icon in the output image.
- You can **rotate** the image for the output picture, or leave it as default.
  - **Auto**: Uses display settings.
  - **Identity**: No rotation is applied. Display settings are ignored.
  - **90**: Forced to 90 degrees.
  - **180**: Forced to 180 degrees.
  - **270**: Forced to 270 degrees.
  
References
----------

[DXGI desktop duplication sample](https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/DXGIDesktopDuplication)

[How to render by using a Direct2D device context](https://docs.microsoft.com/en-us/windows/win32/direct2d/devices-and-device-contexts)

[OBS Studio](https://github.com/obsproject/obs-studio)

[Desktop Screen Capture on Windows](https://www.codeproject.com/Tips/1116253/Desktop-Screen-Capture-on-Windows-via-Windows-Desk)

Build the sample
----------------

To build this sample, open the solution (.sln) file titled dxgi_desktop_capture.sln from Visual Studio 2013 for Windows 8.1 (any SKU) or later versions of Visual Studio and Windows. Press F7 (or F6 for Visual Studio 2013) or go to Build-\>Build Solution from the top menu after the sample has loaded.

Run the sample
--------------

To run this sample after building it, perform the following:

1. Navigate to the directory that contains the new executable, using the command prompt.
2. Type one of the following commands to run the executable.
   1. From the command-line, run **dxgi_desktop_capture.exe -help** to the show help topic. When this command is executed, the output will output.
      ```
      usage: dxgi_desktop_capture.exe [options] -o outfile
      
      Print help / information / capabilities:
      -h topic            show help
      -? topic            show help
      -help topic         show help
      -sources device     list monitors of the dxgi device (in json format)
      -i monitor_idx      The index of the monitor.
      -c show_cursor      show cursor visible in output image. (0:false, 1:true)
      -s size_mode        force image size mode. (0:Normal, 1:Stretch, 2:Auto, 3:Center, 4:Zoom)
      -r rotation_mode    force image rotation mode. (0:Auto, 1:Identity, 2:90, 3:180, 4:270)
      -x image_width      force output image width
      -y image_height     force output image height
      -o outfile          set output image file name (supports: *.bmp; *.png; *.tif)
      -show               show result image file
      ```
   2. From the command-line, run **dxgi_desktop_capture.exe -o .\\output_file_name.bmp** to produce a bitmap. Change the file extension for other supported file formats. (*.png; *.jpg)
   3. If you have more than one desktop monitor, from the command-line for the second monitor, run **dxgi_desktop_capture.exe -i 1 -o .\\output_file_name.jpg** to produce a jpeg.
   4. From the command-line, run **dxgi_desktop_capture.exe -s 4 -x 720 -y 576 -o .\output_file_name.jpg -show 1** to produces a 720x576 jpeg, keeping the desktop aspect ratio.
