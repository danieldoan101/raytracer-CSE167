/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"

// The scene init definition 
#include "RTScene.inl"


using namespace glm;
void RTScene::buildTriangleSoup(void){
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    /*
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for (std::pair<std::string, Light*> entry : light){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }
    */
    // Define stacks for depth-first search (DFS)
    std::stack < RTNode* > dfs_stack;
    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.
    
    // Initialize the current state variable for DFS
    RTNode* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera -> view; // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
    
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    
    // If you want to print some statistics of your scene graph
    // std::cout << "total numb of nodes = " << node.size() << std::endl;
    // std::cout << "total number of edges = " << total_number_of_edges << std::endl;
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top();  matrix_stack.pop();
        // draw all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            // Prepare to draw the geometry. Assign the modelview and the material.
            
            glm::mat4 curModelView = cur_VM * cur->modeltransforms[i];
            glm::mat3 invTransMV = glm::inverse(glm::transpose(mat3(curModelView)));
            Material* curMaterial = cur->models[i]->material;

            /*
            std::cout << curModelView[0][0] << "," << curModelView[1][0] << "," << curModelView[2][0] << "," << curModelView[3][0] << "\n";
            std::cout << curModelView[0][1] << "," << curModelView[1][1] << "," << curModelView[2][1] << "," << curModelView[3][1] << "\n";
            std::cout << curModelView[0][2] << "," << curModelView[1][2] << "," << curModelView[2][2] << "," << curModelView[3][2] << "\n";
            std::cout << curModelView[0][3] << "," << curModelView[1][3] << "," << curModelView[2][3] << "," << curModelView[3][3] << "\n\n";
            */

            // The draw command
            //shader -> setUniforms();
            //( cur -> models[i] ) -> geometry -> draw();
            for( Triangle origTri : cur->models[i]->geometry->elements) {
                Triangle transformedTri;
                
                glm::vec4 currProd = curModelView * glm::vec4(origTri.P[0],1);
                transformedTri.P.push_back(glm::vec3(currProd.x,currProd.y,currProd.z));
                currProd = curModelView * glm::vec4(origTri.P[1],1);
                transformedTri.P.push_back(glm::vec3(currProd.x,currProd.y,currProd.z));
                currProd = curModelView * glm::vec4(origTri.P[2],1);
                transformedTri.P.push_back(glm::vec3(currProd.x,currProd.y,currProd.z));
                /*
                glm::vec3 currProdN = origTri.N[0]*invTransMV;
                transformedTri.N.push_back(currProdN);
                currProdN = origTri.N[1]*invTransMV;
                transformedTri.N.push_back(currProdN);
                currProdN = origTri.N[2]*invTransMV;
                transformedTri.N.push_back(currProdN);
                */
                /*
                transformedTri.P.push_back(origTri.P[0]*curModelView3);
                transformedTri.P.push_back(origTri.P[1]*curModelView3);
                transformedTri.P.push_back(origTri.P[2]*curModelView3);
                */
                transformedTri.N.push_back(invTransMV*origTri.N[0]);
                transformedTri.N.push_back(invTransMV*origTri.N[1]);
                transformedTri.N.push_back(invTransMV*origTri.N[2]);
                
                transformedTri.material = curMaterial;
                triangle_soup.push_back(transformedTri);
            }
        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            matrix_stack.push(cur_VM * cur -> childtransforms[i]);
        }
        
    } // End of DFS while loop.    
}



