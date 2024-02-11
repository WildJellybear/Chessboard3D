#include "../Public/ChessBoard.h"
#include "../Public/Bezier.h"

std::map<int, std::shared_ptr<Model>> ChessBoard::piecesModelsMap = {};

ChessBoard::ChessBoard(std::shared_ptr<Mesh> Mesh, std::shared_ptr<Texture> Tex, glm::vec3 pos) : Model(Mesh, Tex, pos)
{
	m_A1Position = glm::vec3(-3.5f, -1.9f, 3.5f);
}

ChessBoard::~ChessBoard()
{
}

float ChessBoard::GetZ(int i, int j) const
{
	Bezier* bezier = (Bezier*)m_Mesh.get();
	int w = bezier->m_TriangulationPrecision / (SIZE * 2);
	int x = w + w * 2 * i;
	int y = w + w * 2 * (7 - j);
	return bezier->GetVertexZ(x, y);
}

void ChessBoard::Tick(float interval)
{
	((Bezier*)m_Mesh.get())->Tick(interval);
}

void ChessBoard::Draw(const Renderer& renderer, Shader& shader) const
{
	shader.SetUniform4f("u_Color", 0.4f, 0.4f, 0.4f, 1.f);
	Model::Draw(shader);


	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (m_Board[i][j].first == None)
				continue;

		 	ChessBoard::piecesModelsMap[m_Board[i][j].first]->SetPosition(glm::vec3(m_A1Position.x + j*1.f, m_A1Position.y + GetZ(i, j) * 8, m_A1Position.z - i * 1.f));
			shader.Bind();
			if (m_Board[i][j].second == false) // White piece
				shader.SetUniform4f("u_Color", 0.8f, 0.8f, 0.8f, 1.f);
			else // Black piece
				shader.SetUniform4f("u_Color", 0.4f, 0.4f, 0.4f, 1.f);

			ChessBoard::piecesModelsMap[m_Board[i][j].first]->Draw(shader);
		}
	}
}

void ChessBoard::AddPiece(int type, bool colour, int column, int row)
{
	if (type < OT_Pawn || type > OT_King)
		ASSERT("Invalid piece type");
	if (row >= SIZE || row < 0)
		ASSERT("Invalid row in adding piece");
	if (column >= SIZE || column < 0)
		ASSERT("Invalid column in adding piece");

	m_Board[row][column] = std::pair<int, bool>(type, colour);
}
