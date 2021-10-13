# Kaimos Engine
Kaimos Engine is a basic 3D engine for educational purposes. It aims to be a 3D Geometry visualizer with an OpenGL renderer to serve as a basis for the development of the Degree Thesis (TFG) of my degree in Videogames Design and Development at CITM-UPC.

Check the presentation video: https://www.youtube.com/watch?v=nbP7d8Ai8Oo&

Its purposes is to have a a node-based Material Editor in order to make the understanding and creation of materials and environments edition easy for anyone without having a deep understanding of the subject and without having to write any kind of code. As well, my purpose is to gather the documentation to made this in the mentioned thesis (found [here](https://drive.google.com/file/d/1xbDRZB10k20QQi_Jhxhc_i64FvKP7dUs/view?usp=sharing)).

Besides, it supports various rendering techniques of which we would like to higlight PBR and IBL support. Also, it supports models loading from FBX and OBJ extensions (the first might give some problems) and the loading of HDR maps.

## Camera Controls
To move through the scene, shown in the scene viewport, the **editor camera** can be used with the next controls (LMB, RMB and MMB refer to the left, right and middle mouse buttons respectively):

      - Look Around:                   RMB
      - Forward Movement:              LMB
      - Orbit:                         ALT + LMB
      - FPS Movement:                  RMB + WASDQE
      - Double Speed FPS Movement:     RMB + WASDQE + Shift
      - Control FPS Movement Speed:    RMB + Mouse Scroll

      - Zoom:                          ALR + RMB or Mouse Scroll
      - Panning:                       MMB or LMB + RMB
      - Focus:                         F Key (center or selected object)


Some of this controls can be used together. Note that some **options of this camera** can be modified by clicking in a button on top of the upper right corner of the viewport.

A list of the **scene objects** can be seen at the left (*Scene Panel*), which can be clicked and have their **components modified** through the *Properties Panel* (objects parenting is not implemented yet!).
Also, they can be **moved, rotated and scaled** by clicking on them and using the **Guizmo** that will apppear on the viewport. The Guizmo modification operation (move, rotate, scale) can be switched with WER keys or through the buttons on top of the viewport, at the left. Also, this guizmo can be unused by triggering the **selection key**, with Q key or its button on top-left too.

In addition, **snapping** can be used with Guizmo by pressing CTRL key while using itwith its selection button on top of the viewport, in the same place as the Guizmo operation buttons. As well, the **Guizmo Mode** can be switched between Local/Global space with the spacebar key while using it or with its selection button in the same place. 

Note that the panels of the engine can be displaced and docked as the user sees fit.

## Material Editor Controls
If an object has a Sprite Renderer Component attached, the component in the Properties Panel will display a button that will allow the user to open the **Kaimos Material Editor** window. This tool allows to modify the Sprite Renderer Component through a nodal editor, giving better results than the plain component editor.

This window can be moved and docked as any panel, even can be separated as another window. The **controls** of this editor are the next ones (LMB, RMB and MMB refer to the left, right and middle mouse buttons respectively):

      - Create Node:          RMB on Editor (opens a node selector)
      - Delete Node:          Double MMB click on top
      - Delete Link:          Double MMB click on top or ALT + LMB click or drag to empty space
      - Editor Panning:       ALT + LMB or MMB
      - Zoom:                 Not implemented yet

Note that **nodes and links can be selected and moved** with LMB (even more than one with Shift key pressed or multiple selection with LMB drag). The **selections can also be deleted** with SUPR/DEL key.

## Kaimos Engine Core
Kaimos Engine supports, in its first version, and apart of what's already mentioned, a basic events sytem, a geometry loader with full UI Support using Dear ImGui (making use of its plugins for docking and for the node/graph editor), an Entity Component System (ECS) with transformations based on glm math library and a cameras system. Note that some of the mentioned features are currently under development.

This project is licenced under Apache License 2.0 which can be found in the "LICENSE" file in the very same directory than this README file.

# Disclaimer
The core systems of Kaimos engine were made following a series of tutorials from YouTube that can be found [in this YouTube list](https://www.youtube.com/watch?v=JxIZbV_XjAs&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT). For this reason, Kaimos Engine could be considered a derivative work of the [Hazel Engine](https://github.com/TheCherno/Hazel) made in the tutorial series, whose licence requires to retain a copy of the copyright notice of the original work, which can be found in the file named "LICENSE_HAZEL" in the very same directory than this README file.

In addition, the mentioned copyright notice requires to cause any modified files from the original source. These are the whole files, since they were made completely manually by the author of this repository (@lucho1), except that they were made following the aforementioned tutorial (its advices, its patterns...), which can lead to have a very similar code in many of the core files of the Kaimos Engine.

Finally, this repository is the project repository to host the practical or development part of a Degree Thesis (which can be found here) by the owner of this repository, Lucho Suaya, that consists on a Material Editor made on top of the Kaimos Engine. Let us guarantee that the parts concerning the important, key, or central files or systems for the project development (the ones that were object of the thesis) were not, under any circumstances, copies, modified or derivative works from any other source, they were completely made by the author with the investigations made for the thesis.


Kaimos ©2021 by [Luciano Suaya Leiro](https://github.com/lucho1) is licenced under [Apache License 2.0](https://github.com/lucho1/Kaimos/blob/master/LICENSE).


# 3rd Parties
In terms of Software (...)

In terms of art (icons, images, ...), we have used the next resources, all with licence to use them (mostly without comercial attribution).

## Resources
PBR Materials & Textures
  - Learn OpenGL (https://learnopengl.com/)
  - Free PBR (https://freepbr.com/)
  - Polyhaven (https://polyhaven.com/)
  - Quixel Bridge (Megascans - https://quixel.com/bridge)

IBL Textures
  - HDR Labs (http://www.hdrlabs.com/sibl/archive.html)
  - HRRI Hub (https://www.hdri-hub.com/)
  - Polyhaven (https://polyhaven.com/)

## Icons
Toolbar Icons
  - 3D shape by mynamepong from the Noun Project
  - 3D by pictohaven from the Noun Project
  - 3D by mardjoe from the Noun Project
  - Cursor by Johan Cato from the Noun Project
  - Home by Michael Senkow from the Noun Project
  - World by Lorenzo Baldini from the Noun Project
  - Magnet by Ralf Schmitzer from the Noun Project
  - Grid by Andrea Severgnini from the Noun Project
  - Camera by Hakim Fadhlullah from the Noun Project
