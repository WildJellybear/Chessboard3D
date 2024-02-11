#pragma once

#include <map>

#include "Model.h"
#include "memory"
#include "../../enums/ObjectType.h"
#define SIZE 8

class ChessBoard :
    public Model
{
private:

    // columns A-H
    // rows 1-8
    // first - type
    // second - colour (0 - white, 1 - black) 
    std::pair<int, bool> m_Board[SIZE][SIZE] = { std::pair<int, bool>(None, 0) };

    glm::vec3 m_A1Position;

public:
    static std::map<int, std::shared_ptr<Model>> piecesModelsMap;
    
    ChessBoard(std::shared_ptr<Mesh> Mesh, std::shared_ptr<Texture> Tex, glm::vec3 pos);
    ~ChessBoard();

    float GetZ(int i, int j) const;
    void Tick(float interval);

    void AddPiece(int type, bool colour, int row, int column);

    void Draw(const Renderer& renderer, Shader& shader) const;
};

