#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

float sphereRadius = 1.0f;  // Initial radius of the sphere
GLuint texture;              // Texture ID

// Rotation angles
float rotationX = 0.0f;
float rotationY = 0.0f;
int lastMouseX, lastMouseY;
int isDragging = 0;

void loadTexture(const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        fprintf(stderr, "Failed to load texture! SDL_image Error: %s\n", IMG_GetError());
        return;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
}

void drawSphere() {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, sphereRadius, 32, 32);
    gluDeleteQuadric(quadric);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("3D Sphere with Texture",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    loadTexture("pigeon.jpg");

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                isDragging = 1;
                SDL_GetMouseState(&lastMouseX, &lastMouseY);
            }
            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                isDragging = 0;
            }
            if (event.type == SDL_MOUSEMOTION && isDragging) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Calculate the change in mouse position
                int deltaX = mouseX - lastMouseX;
                int deltaY = mouseY - lastMouseY;

                // Update rotation angles
                rotationY += deltaX * 0.5f;  // Rotate around Y-axis
                rotationX += deltaY * 0.5f;  // Rotate around X-axis

                // Store current mouse position
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
        }

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Position the camera
        glTranslatef(0.0f, 0.0f, -5.0f);

        // Apply rotations
        glRotatef(rotationX, 1.0f, 0.0f, 0.0f);  // Rotate around X-axis
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);  // Rotate around Y-axis

        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // Draw the sphere
        drawSphere();

        // Swap the window buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteTextures(1, &texture);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

