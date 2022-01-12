/* Author: Diego Cosin <cosinma@esat-alumni.com>. */

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include "float.h"
#include "hittable_list.h"
#include "sphere.h"
#include "random.h"
#include "camera.h"
#include "material.h"
#include "settings.h"
#include "ppm.h"
#include "aarect.h"
// #include "constant_medium.h"
#include "bvh.h"
#include "pdf.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

void perlin_scene(hittable **scene, hittable **lightarea, camera **view, settings* s) {
    
    texture* color = new constant_texture(vec4(0.5, 0.5, 0.5));
    texture *pertext = new noise_texture(4);
    hittable **shapelist = new hittable*[2];
    hittable **lightlist = new hittable*[1];

    shapelist[0] = new sphere(vec4(0,-1000, 0), 1000, new lambertian(color));
    shapelist[1] = new sphere(vec4(0, 2, 0), 2, new lambertian(pertext));

    lightlist[0] = new sphere(vec4(0, 2, 0), 2, new lambertian(pertext));

    *scene = new hittable_list(shapelist, 2);
    *lightarea = new hittable_list(lightlist,1);

    vec4 lookfrom(13, 2, 3);
    vec4 lookat(0, 0, 0);
    double dist_to_focus = 10.0;
    double aperture = 0.0;
    double vfov = 40.0;
    double aspect = double(s->window_width) / double(s->window_height);
    *view = new camera(lookfrom, lookat, vec4(0,1,0), vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);

}
void random_scene(hittable **scene, hittable **lightarea, camera **view, settings* s) {

  int i = 0;
  int l = 0;
  hittable **shapelist = new hittable*[500];
  hittable **lightlist = new hittable*[500];

  texture *checker = new checker_texture(
    new constant_texture(vec4(0.2, 0.3, 0.1)),
    new constant_texture(vec4(0.9, 0.9, 0.9)));

  shapelist[i++] = new sphere(vec4(0,-1000,0), 1000, new lambertian(checker));
  for (int a = -10; a < 10; a++) {
    for (int b = -10; b < 10; b++) {
      double choose_mat = random_double();
      vec4 center(a+0.9*random_double(),0.2,b+0.9*random_double());
      if ((center-vec4(4.0,0.2,0.0)).length() > 0.9) {
        if (choose_mat < 0.5) {
          shapelist[i++] = new sphere(center, 0.2,
            new lambertian(new constant_texture(vec4(
                                random_double()*random_double(),
                                random_double()*random_double(),
                                random_double()*random_double()))
            )
          );
        }
        else if (choose_mat < 0.75) { // light
          shapelist[i++] = new sphere(center, 0.2,
            new diffuse_light(new constant_texture(vec4(
                                random_double()*random_double(),
                                random_double()*random_double(),
                                random_double()*random_double()))
            )
          );
          lightlist[l++] = new sphere(center, 0.2,
            new diffuse_light(new constant_texture(vec4(
                                random_double()*random_double(),
                                random_double()*random_double(),
                                random_double()*random_double()))
            )
          );
        }
        else if (choose_mat < 0.8) {
          shapelist[i++] = new moving_sphere(center,
            center+vec4(0.0,0.5*random_double(),0.0),0.0,1.0, 0.2,
            new lambertian(new constant_texture(vec4(
                                random_double()*random_double(),
                                random_double()*random_double(),
                                random_double()*random_double()))
            )
          );
        }
        else if (choose_mat < 0.95) { // metal
          shapelist[i++] = new sphere(center, 0.2,
            new metal(vec4(0.5*(1.0 + random_double()),
                           0.5*(1.0 + random_double()),
                           0.5*(1.0 + random_double())),
                           0.5*random_double()));
        }
        else { // glass
           shapelist[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }

  shapelist[i++] = new sphere(vec4(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec4(0.4, 0.2, 0.1))));
  shapelist[i++] = new sphere(vec4( 4, 1, 0), 1.0, new dielectric(1.5));
  lightlist[l++] = new sphere(vec4( 4, 1, 0), 1.0, new dielectric(1.5));

  ppm image("../data/test_images/earthmap.ppm");

  shapelist[i++] = new sphere(vec4(0, 1, 0), 1.0, new lambertian(new image_texture(image.pix, image.width, image.height)));
  shapelist[i++] = new sphere(vec4(0, 3, 0), 0.25, new diffuse_light(new constant_texture(vec4(4.0,4.0,4.0))));
  shapelist[i++] = new sphere(vec4(8, 1, 0), 1.0,new metal(vec4(0.7, 0.6, 0.5), 0.0));

    *scene = new hittable_list(shapelist,i);
    *lightarea = new hittable_list(lightlist,l);

    vec4 lookfrom(13,2,3);
    vec4 lookat(0,0,0);
    double dist_to_focus = 10.0;
    double aperture = 0.1;
    double vfov = 40.0;
    double aspect = double(s->window_width) / double(s->window_height);
    *view = new camera(lookfrom, lookat, vec4(0,1,0), vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}
void cornell_box(hittable **scene, hittable **lightarea, camera **view, settings* s) {

  int i = 0;
  int l = 0;
  hittable **shapelist = new hittable*[8];
  hittable **lightlist = new hittable*[8];
  material *red   = new lambertian(new constant_texture(vec4(0.65, 0.05, 0.05)));
  material *white = new lambertian(new constant_texture(vec4(0.73, 0.73, 0.73)));
  material *green = new lambertian(new constant_texture(vec4(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(vec4(15.0, 15.0, 15.0)));
  material *glass = new dielectric(1.5);
  shapelist[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  shapelist[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  shapelist[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554,light));
  lightlist[l++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554,light));
  shapelist[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  shapelist[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  shapelist[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
  shapelist[i++] = new translate(new rotate_y(new box(vec4(0, 0, 0), vec4(165, 330, 165), white),  15), vec4(265,0,295));
  shapelist[i++] = new sphere(vec4(190, 90, 190),90 , glass);
  lightlist[l++] = new sphere(vec4(190, 90, 190),90 , glass);

  *scene = new hittable_list(shapelist,i);
  *lightarea = new hittable_list(lightlist,l);

  vec4 lookfrom(278, 278, -800);
  vec4 lookat(278, 278, 0);
  double dist_to_focus = 10.0;
  double aperture = 0.0;
  double vfov = 40.0;
  double aspect = double(s->window_width) / double(s->window_height);
  *view = new camera(lookfrom, lookat, vec4(0,1,0), vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

inline vec4 de_NaN(const vec4& c) {
  vec4 temp = c;
  if (!(temp[0] == temp[0])) temp[0] = 0.0;
  if (!(temp[1] == temp[1])) temp[1] = 0.0;
  if (!(temp[2] == temp[2])) temp[2] = 0.0;
  return temp;
}

inline vec4 filter_NaN(const vec4& c, uint32_t& discarded_samples) {
  if (!(c[0] == c[0])) { discarded_samples++; return vec4(0.0, 0.0, 0.0); }
  if (!(c[1] == c[1])) { discarded_samples++; return vec4(0.0, 0.0, 0.0); }
  if (!(c[2] == c[2])) { discarded_samples++; return vec4(0.0, 0.0, 0.0); }
  return c;
}

inline bool check_NaN(const vec4& c) {
  return !(c[0] == c[0]) || !(c[1] == c[1]) || !(c[2] == c[2]);
}

// Globals because I can

double inv_width;
double inv_height;

vec4 color(const ray& r, hittable *world, hittable *light, int depth) {
  hit_record hrec;
  if (world->hit(r, 0.001, DBL_MAX, hrec)) {
    scatter_record srec;
    vec4 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);

    // Depth per ray
    if (depth < 3 && hrec.mat_ptr->scatter(r, hrec, srec)) {
      if (srec.is_specular) {
        return srec.attenuation * color(srec.specular_ray, world, light, depth+1);
      }
      else {
        hittable_pdf plight(light, hrec.p);
        mixture_pdf p(&plight, srec.pdf_ptr);
        ray scattered = ray(hrec.p, p.generate(), r.time());
        double pdf_val = fmax(p.value(scattered.direction()), DBL_MIN);
        delete srec.pdf_ptr;
        return emitted + srec.attenuation * hrec.mat_ptr->scattering_pdf(r,hrec,scattered) * color(scattered, world, light, depth+1) / pdf_val;
      }
    }
    else {
        return emitted;
    }
  }
  else {
    double t = 0.5*((r.direction()).normalized().y + 1.0);
    return vec4(1.0, 1.0, 1.0)*(1.0-t) + vec4(0.5, 0.7, 1.0)*t;
    //return vec4(0.0, 0.0, 0.0);
  }
}

inline bool update_state(uint16_t& px, uint16_t& py, uint16_t accuracy, settings* s, float* framebuffer) {
  if (px < s->window_width - accuracy) {
    px += accuracy;
    if (framebuffer[(s->window_width * py + px) * 4 + 3] == 1.0) {
      px += accuracy;
    }
  }
  else if (py < s->window_height - accuracy) { px = 0; py += accuracy; }
  else { return true; }
  return false;
}

// THREADING
static std::atomic<int> nWorkerComplete;
struct WorkerThread {
  uint16_t px = 0;
  uint16_t py = 0;
  uint16_t accuracy = 0;

  settings* s = nullptr;
  camera* view = nullptr;
  hittable* world = nullptr;
  hittable* light = nullptr;
  float* framebuffer = nullptr;

  bool alive = true;
  std::condition_variable cvStart;
  std::mutex mux;

  std::thread thread;

  void Start(uint16_t x, uint16_t y, uint16_t acc) {
    px = x;
    py = y;
    accuracy = acc;

    std::unique_lock<std::mutex> lm(mux);
    cvStart.notify_one();
  }

  void render_pixel() {

    while (alive) {
      std::unique_lock<std::mutex> lm(mux);
      cvStart.wait(lm);

      framebuffer[(s->window_width * py + px) * 4 + 3] = 1.0;
      vec4 col = vec4(0.0, 0.0, 0.0);
      vec4 c;
      //uint32_t discarded_samples = -1;
      for (uint32_t samples = 0; samples < s->num_samples; samples++) {
        do {
          double u = double(px + random_double()) * inv_width;
          double v = double(s->window_height - py + random_double()) * inv_height;

          c = color(view->get_ray(u, v), world, light, 0);
          //discarded_samples++;
        } while (check_NaN(c));

        //if (discarded_samples > 0) printf("Discarded %d samples\n", discarded_samples);
        col += c;
      }

      col /= (double(s->num_samples));
      //col *= s->inv_num_samples; Doesn't really make a difference
      col = col.square_root();

      for (int k = px; k < px + accuracy && k < s->window_width; ++k) {
        for (int l = py; l < py + accuracy && l < s->window_height; ++l) {
          framebuffer[(s->window_width * l + k) * 4 + 0] = col.r;
          framebuffer[(s->window_width * l + k) * 4 + 1] = col.g;
          framebuffer[(s->window_width * l + k) * 4 + 2] = col.b;
          framebuffer[(s->window_width * l + k) * 4 + 3] = 0.99;
          // Different alpha to check pixels that have already been rendered in broad passes
        }
      }
      /*framebuffer[(s->window_width * py + px) * 4 + 0] = col.r;
      framebuffer[(s->window_width * py + px) * 4 + 1] = col.g;
      framebuffer[(s->window_width * py + px) * 4 + 2] = col.b;*/
      framebuffer[(s->window_width * py + px) * 4 + 3] = 1.0;
      
      //if (update_state(px, py, accuracy, s, framebuffer)) 
        nWorkerComplete++;
    }
  }
};
const int nMaxThreads = 32;
WorkerThread workers[nMaxThreads];
void cleanup_workers() {
  for (int i = 0; i < nMaxThreads; i++)
  {
    workers[i].alive = false;		 // Allow thread exit
    workers[i].cvStart.notify_one(); // Fake starting gun
  }

  // Clean up worker threads
  for (int i = 0; i < nMaxThreads; i++)
    workers[i].thread.join();

}
void initialize_workers(camera* view, hittable* world, hittable* light, settings* s, float* framebuffer) {
  for (int i = 0; i < nMaxThreads; ++i) {
    workers[i].alive = true;

    workers[i].s = s;
    workers[i].view = view;
    workers[i].world = world;
    workers[i].light = light;
    workers[i].framebuffer = framebuffer;

    workers[i].thread = std::thread(&WorkerThread::render_pixel, &workers[i]);
  }
}



void render_pass(GLuint shaderProgram, GLFWwindow* window, uint16_t accuracy, camera *view, hittable *world, hittable *light, settings* s, float* framebuffer) {

  uint16_t px = 0;
  uint16_t py = 0;
  bool done = false;

  while(!glfwWindowShouldClose(window) && !done) {

    nWorkerComplete = 0;
    int nUsedWorker = 0;
    for (int i = 0; i < nMaxThreads; ++i) {

      // Assign threads to pixels
      workers[i].Start(px, py, accuracy);
      nUsedWorker++;

      // Update next pixel to be rendered
      if (update_state(px, py, accuracy, s, framebuffer)) { done = true; break; }
    }

    while (nWorkerComplete < nUsedWorker) // Wait for all workers to complete
    {    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, s->window_width, s->window_height, GL_RGBA, GL_FLOAT, framebuffer);
    glUseProgram(shaderProgram);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
  }
}

int main(int argc, char** argv) {

  srand((unsigned)time(NULL));
  settings s = ReadSettings();

  inv_width = 1.0 / double(s.window_width);
  inv_height = 1.0 / double(s.window_height);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(s.window_width, s.window_height, "RayTracerV2", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, s.window_width, s.window_height);


  hittable *world = nullptr;
  hittable *light = nullptr;
  camera *view = nullptr;

  switch (s.scene_index) {
    case 0:
      random_scene(&world,&light, &view, &s);
      break;
    case 1:
      cornell_box(&world,&light, &view, &s);
      break;
    case 2:
      perlin_scene(&world,&light, &view, &s);
      break;
    default:
      random_scene(&world, &light, &view, &s);
      break;
  }

  // OPENGL STUFF

  float* framebuffer = (float*)calloc(4 * s.window_width * s.window_height, sizeof(float));

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s.window_width, s.window_height, 0, GL_RGBA, GL_FLOAT, framebuffer);

  const char* vertex_shader_source = {
    "#version 330\n"
    "layout(location = 0) in vec3 aPosition;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = vec4(aPosition,1.0);\n"
    " texCoord = aTexCoord;"
    "}\n"
  };

  const char* fragment_shader_source = {
    "#version 330\n"
    "out vec4 fragColor;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D colorTexture;\n"
    "void main()\n"
    "{\n"
    "	fragColor = texture(colorTexture, texCoord);\n"
    "}\n"
  };
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  // link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  float vertices[] = {
    -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
     1.0f,  1.0f, 0.0f,     1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,     0.0f, 0.0f
  };
  GLuint indices[] = {
      0, 1, 2,
      0, 2, 3
  };
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  

  // NOW THE FUN STUFF BEGINS
  initialize_workers(view, world, light, &s, framebuffer);

  auto t_start = std::chrono::high_resolution_clock::now();
  auto t_finish = std::chrono::high_resolution_clock::now();
  if (s.progressive_render){
    uint16_t pass_accuracy = s.window_width < s.window_height ? s.window_width : s.window_height;
    while (pass_accuracy > 1) {
      std::cout << "Performing broad pass " << pass_accuracy << std::endl;
      t_start = std::chrono::high_resolution_clock::now();
      render_pass(shaderProgram, window, pass_accuracy, view, world, light, &s, framebuffer);
      t_finish = std::chrono::high_resolution_clock::now();
      std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_finish - t_start).count() / 1000.0 << " seconds." << std::endl;
      pass_accuracy >>= 1;
    }
  }
  std::cout << "Performing final pass" << std::endl;
  t_start = std::chrono::high_resolution_clock::now();
  render_pass(shaderProgram, window, 1, view, world, light, &s, framebuffer);
  t_finish = std::chrono::high_resolution_clock::now();
  std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_finish - t_start).count() / 1000.0 << " seconds." << std::endl;

  cleanup_workers();
  getchar();
  if (s.output_to_file) {
    uint8_t* write_buffer = (uint8_t*)calloc(3 * s.window_width * s.window_height, sizeof(uint8_t));

    for (int k = 0; k < s.window_height; ++k) {
      for (int l = 0; l < s.window_width; ++l) {
        write_buffer[(s.window_width * k + l) * 3 + 0] = uint8_t(fmin(framebuffer[(s.window_width * k + l) * 4 + 0], 1.0f) * 255.99f);
        write_buffer[(s.window_width * k + l) * 3 + 1] = uint8_t(fmin(framebuffer[(s.window_width * k + l) * 4 + 1], 1.0f) * 255.99f);
        write_buffer[(s.window_width * k + l) * 3 + 2] = uint8_t(fmin(framebuffer[(s.window_width * k + l) * 4 + 2], 1.0f) * 255.99f);
      }
    }
    stbi_write_png("../data/render.png", s.window_width, s.window_height, 3, write_buffer, 3 * s.window_width);
    free(write_buffer);
  }
  free(framebuffer);
  getchar();
  glfwTerminate();
  return 0;
}