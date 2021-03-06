//https://github.com/awwdev

#pragma once
#include "app/Editor/EditorCommandDirection.hpp"

#include "app/Editor/Commands/EditorCmd_PrefabPlacement.hpp"
#include "app/Editor/Commands/EditorCmd_TerrainFacePaint.hpp"
#include "app/Editor/Commands/EditorCmd_TerrainVertexMove.hpp"
#include "app/Editor/Commands/EditorCmd_TerrainVertexPaint.hpp"

namespace rpg::app {

enum class EditorCommandEnum
{
    EditorCmd_PrefabPlacement,
    EditorCmd_TerrainFacePaint,
    EditorCmd_TerrainVertexMove,
    EditorCmd_TerrainVertexPaint
};

struct EditorCommand
{
    EditorCommandEnum editorCommandEnum;
    union
    {
        EditorCmd_PrefabPlacement    cmd_prefabPlacement;
        EditorCmd_TerrainFacePaint   cmd_terrainFacePaint;
        EditorCmd_TerrainVertexMove  cmd_terrainVertexMove;
        EditorCmd_TerrainVertexPaint cmd_terrainVertexPaint;
    };

    EditorCommand() {} 

    template<EditorCommandDirection DIR>
    void Execute(res::Resources& res, ecs::ECS& ecs)
    {
        switch(editorCommandEnum)
        {
            case EditorCommandEnum::EditorCmd_PrefabPlacement:      cmd_prefabPlacement.Execute<DIR>(res, ecs); break;
            case EditorCommandEnum::EditorCmd_TerrainFacePaint:     cmd_terrainFacePaint.Execute<DIR>(res, ecs); break;
            case EditorCommandEnum::EditorCmd_TerrainVertexMove:    cmd_terrainVertexMove.Execute<DIR>(res, ecs); break;
            case EditorCommandEnum::EditorCmd_TerrainVertexPaint:   cmd_terrainVertexPaint.Execute<DIR>(res, ecs); break;
        }
    }
};

template<typename T>
EditorCommand CreateEditorCommand(T const& cmd)
{
    //initializes the union and the enum
    EditorCommand editorCmd;

    if constexpr (std::is_same_v<T, EditorCmd_PrefabPlacement>)
    {
        editorCmd.editorCommandEnum = EditorCommandEnum::EditorCmd_PrefabPlacement;
        editorCmd.cmd_prefabPlacement = cmd;
    }

    if constexpr (std::is_same_v<T, EditorCmd_TerrainFacePaint>)
    {
        editorCmd.editorCommandEnum = EditorCommandEnum::EditorCmd_TerrainFacePaint;
        editorCmd.cmd_terrainFacePaint = cmd;
    }

    if constexpr (std::is_same_v<T, EditorCmd_TerrainVertexMove>)
    {
        editorCmd.editorCommandEnum = EditorCommandEnum::EditorCmd_TerrainVertexMove;
        editorCmd.cmd_terrainVertexMove = cmd;
    }

    if constexpr (std::is_same_v<T, EditorCmd_TerrainVertexPaint>)
    {
        editorCmd.editorCommandEnum = EditorCommandEnum::EditorCmd_TerrainVertexPaint;
        editorCmd.cmd_terrainVertexPaint = cmd;
    }

    return editorCmd;
}

}//ns