#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO create the cells
	FIntVector Origin = pWorld->OriginLocation;
	for (int row = 0; row < Rows; ++row)
	{
		for (int col = 0; col < Cols; ++col)
		{
			
			Cells.emplace_back(Cell(col * CellWidth - SpaceWidth/2, row * CellHeight - SpaceHeight/2, CellWidth, CellHeight));
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO Add the agent to the correct cell
	int cellIndex = PositionToIndex(Agent.GetPosition());
	Cells[cellIndex].Agents.emplace_back(&Agent);
	
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	int oldIndex = PositionToIndex(OldPos);
	int newIndex = PositionToIndex(Agent.GetPosition());

	// If the agent moved to another cell
	if (oldIndex != newIndex)
	{
		// Remove from old cell
		Cells[oldIndex].Agents.remove(&Agent);

		// Add to new cell
		Cells[newIndex].Agents.push_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
	NrOfNeighbors = 0;

	FVector2D agentPos = Agent.GetPosition();

	FRect neighborRect;
	neighborRect.Min = FVector2D(agentPos.X - QueryRadius, agentPos.Y - QueryRadius);
	neighborRect.Max = FVector2D(agentPos.X + QueryRadius, agentPos.Y + QueryRadius);

	for (Cell& cell : Cells)
	{
		if (DoRectsOverlap(cell.BoundingBox, neighborRect))
		{
			for (ASteeringAgent* otherAgent : cell.Agents)
			{
				if (otherAgent == &Agent) continue;

				float distance = FVector2D::Distance(otherAgent->GetPosition(), agentPos);

				if (distance <= QueryRadius)
				{
					Neighbors[NrOfNeighbors] = otherAgent;
					++NrOfNeighbors;
				}
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
	for (const Cell& cell : Cells)
	{
		const std::vector<FVector2D> points = cell.GetRectPoints();

		for (size_t i = 0; i < points.size(); ++i)
		{
			FVector start(points[i].X, points[i].Y, 90.f);
			FVector end(
				points[(i + 1) % points.size()].X,
				points[(i + 1) % points.size()].Y,
				90.f);

			DrawDebugLine(pWorld, start, end, FColor::White, false, -1.f, 0, 1.5f);
		}

		FVector2D textPos2D = points[1]; // top-left
		FVector textPos(textPos2D.X, textPos2D.Y, 20.f);

		DrawDebugString(
			pWorld,
			textPos,
			FString::FromInt(cell.Agents.size()),
			nullptr,
			FColor::White,
			0.f);
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position
	float halfWidth = SpaceWidth * 0.5f;
	float halfHeight = SpaceHeight * 0.5f;

	int colIndex = FMath::Clamp(
		int((Pos.X + halfWidth) / CellWidth),
		0,
		NrOfCols - 1
	);

	int rowIndex = FMath::Clamp(
		int((Pos.Y + halfHeight) / CellHeight),
		0,
		NrOfRows - 1
	);

	return rowIndex * NrOfCols + colIndex;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}