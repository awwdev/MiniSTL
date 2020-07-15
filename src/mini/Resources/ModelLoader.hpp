//https://github.com/awwdev
#pragma once

#include "mini/Utils/Types.hpp"
#include "mini/Utils/Structs.hpp"
#include "mini/Box/Array.hpp"
#include <fstream>
#include <filesystem>

namespace mini::res
{
    //specific blender export!
    inline void LoadModel(box::IArray<utils::VertexDefault>& vertices, chars_t path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) ERR("cannot open file");

        char line [100];
        while(file.getline(line, 100))
        {
            u32 commaCount = 0;
            u32 i1 = 0; 

            math::Vec4f position {};
            position[0][3] = 1;
            math::Vec4f color {};
            math::Vec4f normals {};

            //POSITION
            for(u32 i2 = 0; i2 < 100; ++i2){
                if (commaCount < 3)
                {
                    if(line[i2] == ','){
                        if (commaCount == 0)
                            position[Vx] = (float)std::atof(line+i1);
                        if (commaCount == 1)
                            position[Vy] = (float)std::atof(line+i1);
                        if (commaCount == 2)
                            position[Vz] = (float)std::atof(line+i1);
                        i1 = i2+1;
                        commaCount++;
                    }
                }
                else if (commaCount < 6) 
                {
                    if(line[i2] == ','){
                        color[0][commaCount-3] = (float)std::atof(line+i1);
                        i1 = i2+1;
                        commaCount++;
                    }
                }
                else if (commaCount < 9) 
                {
                    if(line[i2] == ','){
                        normals[0][commaCount-6] = (float)std::atof(line+i1);
                        i1 = i2+1;
                        commaCount++;
                    }
                }
                
            }

            vertices.Append(position, normals, color);
        }

    }

}//ns