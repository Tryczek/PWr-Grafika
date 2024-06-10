
# README.md

## Overview

Welcome to the Computer Graphics and Human-Computer Communication repository for the course led by Jan Nikodem at Wrocław University of Science and Technology. This repository provides a hands-on experience with C++ and OpenGL, focusing on rendering 2D and 3D graphics and incorporating elements of human-computer communication, including user interface design and interactive features.

## Repository Structure

The repository is organized into several folders, each corresponding to a different lab project. Each lab explores various aspects of computer graphics, providing a comprehensive understanding of both fundamental and advanced concepts.

### Folder Structure

- **Lab2**
  - `freeglut/`
  - `freeglut.dll`
  - `Glab2.dev`
  - `Glab2.exe`
  - `Glab2.layout`
  - `Sprawozdanie.pdf`
  - `main.cpp`
  - `main.o`
  - `Makefile.win`
  
- **Lab3**
  - `freeglut/`
  - `freeglut.dll`
  - `Glab2.dev`
  - `Glab2.exe`
  - `Glab2.layout`
  - `Sprawozdanie.pdf`
  - `main.cpp`
  - `main.o`
  - `Makefile.win`
  
- **Lab4**
  - `freeglut/`
  - `freeglut.dll`
  - `Glab2.dev`
  - `Glab2.exe`
  - `Glab2.layout`
  - `main.cpp`
  - `main.o`
  - `Makefile.win`
  - `Sprawozdanie.pdf`
  
- **Lab5**
  - `freeglut/`
  - `tekstury/`
  - `freeglut.dll`
  - `Glab2.dev`
  - `Glab2.exe`
  - `Glab2.layout`
  - `Glab6.docx`
  - `main.cpp`
  - `main.o`
  - `Makefile.win`
  - `Sprawozdanie.pdf`

## Lab Projects

### Lab 2

**Objective:** Explore the basics of OpenGL and C++ programming for rendering simple graphics.

**Key Features:**
- **Coordinate System Setup:** Learn how to set up a basic coordinate system in OpenGL.
- **Simple Shapes:** Render basic shapes such as points and lines.
- **User Interaction:** Implement basic user interaction using keyboard and mouse events.

**Key Files:**
- `main.cpp`: Contains the main logic for rendering 3D objects using OpenGL. It sets up the coordinate system, handles user inputs, and renders shapes.
- `Glab2.exe`: Compiled executable for the project, allowing you to run the application directly.
- `Sprawozdanie.pdf`: Documentation file explaining the project details, including setup instructions, code explanations, and expected outcomes.

### Lab 3

**Objective:** Build on the concepts from Lab 2 by adding more complex graphical elements and user interaction.

**Key Features:**
- **3D Shapes:** Introduce 3D shapes such as cubes and spheres.
- **Camera Controls:** Implement basic camera controls to navigate around the 3D scene.
- **Dynamic Interaction:** Enhance user interaction with more complex events, such as rotating and zooming the view.

**Key Files:**
- `main.cpp`: Enhanced to include more advanced graphics and interaction. It adds 3D shape rendering and camera control functionalities.
- `Glab2.exe`: Updated executable with new features, reflecting the enhancements made in the source code.
- `Sprawozdanie.pdf`: Documentation explaining the new additions and features, providing detailed descriptions and usage instructions.

### Lab 4

**Objective:** Implement advanced techniques in OpenGL, including lighting and texture mapping.

**Key Features:**
- **Lighting:** Learn how to implement different lighting models, including ambient, diffuse, and specular lighting.
- **Textures:** Apply textures to 3D objects to give them a more realistic appearance.
- **Advanced User Controls:** Introduce advanced user controls to manipulate the lighting and textures in real-time.

**Key Files:**
- `main.cpp`: Includes implementations for lighting and textures. It demonstrates how to apply different lighting models and textures to 3D objects.
- `Sprawozdanie.pdf`: Report detailing the methodologies and results of the lab project. It includes theoretical explanations, implementation details, and outcome analysis.

### Lab 5

**Objective:** Finalize the graphics project with comprehensive features, including complex models and interactive user interfaces.

**Key Features:**
- **Complex Models:** Render complex 3D models such as teapots and pyramids.
- **Interactive UI:** Develop an interactive user interface to control various aspects of the graphics application.
- **Comprehensive Integration:** Integrate all learned concepts into a single, comprehensive application.

**Key Files:**
- `main.cpp`: Final project code with complete features, integrating complex models and an interactive user interface.
- `Glab6.docx`: Project documentation, providing a detailed overview of the project, implementation steps, and user instructions.
- `Sprawozdanie.pdf`: Final report summarizing the project. It includes a comprehensive analysis of the project's goals, methodologies, and results.

## Getting Started

To get started with any of the lab projects:

1. **Clone the Repository:**
   ```bash
   git clone <repository-url>
   cd <repository-folder>
   ```

2. **Navigate to the Desired Lab Folder:**
   ```bash
   cd Lab2  # or Lab3, Lab4, Lab5
   ```

3. **Compile the Project:**
   Depending on your environment, use the provided `Makefile` or your preferred method to compile the `main.cpp` file.

4. **Run the Executable:**
   ```bash
   ./Glab2.exe  # Ensure you are in the correct directory and have the necessary permissions.
   ```

## Prerequisites

- **OpenGL**: Ensure you have OpenGL installed on your system.
- **GLUT**: The projects use the freeglut library. Ensure it is installed and correctly referenced in your development environment.
- **C++ Compiler**: A standard C++ compiler is required to compile the source code.

## Additional Resources

- **Documentation**: Each lab folder contains a PDF or DOCX file with detailed explanations of the lab objectives, methodology, and results.
- **Source Code**: The `main.cpp` file in each lab folder contains the primary source code for that lab's project.

## Conclusion

This repository is an excellent resource for anyone interested in learning about computer graphics and human-computer interaction using C++ and OpenGL. Whether you are a student of the course or an enthusiast, you will find valuable insights and practical experience from the projects included.

For any questions or further assistance, please refer to the documentation provided within each lab folder. Happy coding!
