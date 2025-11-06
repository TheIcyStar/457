/*
    Alexander Petrov
    aop5448

    contains load_data() performance improvements, Test models loaded instantly during my testing.
    There's two implementation of reading the 'f' lines,
    the first only reads only the vertices
    and the second reads all vertices/texture/normal values
    the second implementation is commented out because the texture/normal values are not implemented anyways.
*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <limits>
#include "model.h"

#include <ctime>


Model::Model(const char *filename)
    : verts{}, faces{}
{
    load_data(filename);
}

Model::~Model()
{ }


int Model::num_vertices()
{
    return static_cast<int>(verts.size());
}

int Model::num_faces()
{
    return static_cast<int>(faces.size());
}


// These 2 need range error checking

vec3 Model::vertex(int i)
{
    return verts[i];
}

std::vector<int> Model::face(int i)
{
    return faces[i];
}


// private method
void Model::load_data(const char *filename)
{
    std::cout << "\nloading data (v3)..." << std::flush;

    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
        std::cout << "failed to open \"" << filename << "\"...\n";
        return;
    }

    // read the whole file for efficiency
    std::string data;
    in.seekg(0, std::ios::end);
    data.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&data[0], data.size());
    in.close();

    std::istringstream str_in(data);

    std::string ignoreStr;
    std::string line;
    std::string mode;
    std::vector<int> faceVertexes;

    float x_max = -std::numeric_limits<float>::max();
    float y_max = -std::numeric_limits<float>::max();
    float z_max = -std::numeric_limits<float>::max();
    float x_min =  std::numeric_limits<float>::max();
    float y_min =  std::numeric_limits<float>::max();
    float z_min =  std::numeric_limits<float>::max();

    while (std::getline(str_in, line)) {
        std::istringstream ss(line.c_str());
        mode = "";
        ss >> mode;

        if(mode == "v"){
            // vertex definition: "v x y z"
            vec3 v;
            ss >> v.x >> v.y >> v.z;
            verts.push_back(v);

            x_max = std::max(x_max, v.x);
            y_max = std::max(y_max, v.y);
            z_max = std::max(z_max, v.z);
            x_min = std::min(x_min, v.x);
            y_min = std::min(y_min, v.y);
            z_min = std::min(z_min, v.z);

        } else if (mode == "f"){
            std::string tuple;
            std::vector<int> face;
            // face definitions
            // Only vertex data:            "f v1 v2 v3 ..."
            // Vertex and texture coords:   "f v1/vt1 v2/vt2 v3/vt3 ..."
            // Vertex, texture, normals:    "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ..."
            // Vertex and normals:          "f v1//vn1 v2//vn2 v3//vn3 .."

            //Only supports verticies, read the first float and skip until the next whitespace
            //*
            while(ss >> tuple){
                std::istringstream tuplestream(tuple);
                std::string part;

                int vert;
                tuplestream >> vert;
                face.push_back(vert - 1);
            }
            //*/

            //All formats
            //only tested with CMPSC 457's .obj
            /*
            while(ss >> tuple){
                std::istringstream tuplestream(tuple);
                std::string part;
                int vert, texture, normal;

                std::getline(tuplestream, part, '/');
                vert = std::stoi(part);

                if(std::getline(tuplestream, part, '/') && !part.empty()){
                    texture = std::stoi(part);
                }

                if(std::getline(tuplestream, part, '/') && !part.empty()){
                    normal = std::stoi(part);
                }

                face.push_back(vert - 1);
            }
            //*/

            // Triangulate the faces
            if(face.size() == 3){
                faces.push_back(face);

            } else if(face.size() > 3){
                for (unsigned int i = 1; i < face.size() - 1; i++) {
                    faces.push_back(std::vector<int>{ face[0], face[i], face[i + 1] });
                }
            } else {
                std::cerr << ".obj file contains a face with " << face.size() << " faces, skipping" << std::endl;
            }
        }
    }

    // scale vertex coordinates to the canonical volume
    if (x_min < -1.0 || x_max > 1.0 ||
        y_min < -1.0 || y_max > 1.0 ||
        z_min < -1.0 || z_max > 1.0) {
        std::cout << "recentering and scaling...";
        float max_range = std::max({ x_max - x_min, y_max - y_min, z_max - z_min });
        float inv_max_range_2 = 2.0f / max_range;  // so that bounding box is [-1,1]^3

        float x_center = (x_max + x_min) * 0.5f;
        float y_center = (y_max + y_min) * 0.5f;
        float z_center = (z_max + z_min) * 0.5f;
        vec3 center{ x_center, y_center, z_center };

        for (auto v_it = verts.begin(); v_it != verts.end(); v_it++)
            *v_it = (*v_it - center) * inv_max_range_2;
    }
    else {
        std::cout << "nothing to scale...";
    }

    std::cout << "done" << std::endl;
    std::cout << "-- stats\n";
    std::cout << "     number of vertices: " << num_vertices() << std::endl;
    std::cout << "        number of faces: " << num_faces() << std::endl;
    std::cout << std::endl;
}
