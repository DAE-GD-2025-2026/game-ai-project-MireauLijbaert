#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		std::vector<NavLine> portals{};

		if (Path.empty())
			return portals;

		// Degenerate start portal
		portals.emplace_back(NavLine{ Path[0]->GetPosition(), Path[0]->GetPosition() });

		// Intermediate portals
		for (size_t i = 1; i < Path.size() - 1; ++i)
		{
			auto* pNode = static_cast<NavGraphNode*>(Path[i]);
			const int edgeIdx = pNode->GetEdgeIdx();

			if (edgeIdx == Graphs::InvalidNodeId)
				continue;

			const auto& edge = NavPoly.GetEdges()[edgeIdx];

			const FVector2D edgeP1{ edge.GetP1(NavPoly) };
			const FVector2D edgeP2{ edge.GetP2(NavPoly) };
			const FVector2D centerLine = (edgeP1 + edgeP2) * 0.5f;

			const FVector2D previousPosition = Path[i - 1]->GetPosition();

			auto Cross = [](const FVector2D& a, const FVector2D& b) -> float
			{
				return a.X * b.Y - a.Y * b.X;
			};

			const float cp = Cross(centerLine - previousPosition, edgeP1 - previousPosition);

			// p1 should be RIGHT point, p2 should be LEFT point
			if (cp > 0.0f)
			{
				portals.emplace_back(NavLine{ edgeP2, edgeP1 });
			}
			else
			{
				portals.emplace_back(NavLine{ edgeP1, edgeP2 });
			}
		}

		// Degenerate end portal
		portals.emplace_back(NavLine{ Path.back()->GetPosition(), Path.back()->GetPosition() });

		return portals;
	}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> path{};

	if (Portals.empty())
		return path;

	auto Cross = [](const FVector2D& a, const FVector2D& b) -> float
	{
		return a.X * b.Y - a.Y * b.X;
	};

	const unsigned int amtPortals{ static_cast<unsigned int>(Portals.size()) };

	int apexIdx{ 0 };
	int leftLegIdx{ 1 };
	int rightLegIdx{ 1 };

	FVector2D apexPos = Portals[apexIdx].P1;
	FVector2D rightLeg = Portals[rightLegIdx].P1 - apexPos;
	FVector2D leftLeg = Portals[leftLegIdx].P2 - apexPos;

	path.emplace_back(apexPos);

	for (unsigned int portalIdx = 1; portalIdx < amtPortals; ++portalIdx)
	{
		const auto& portal = Portals[portalIdx];

		// --- RIGHT CHECK ---
		FVector2D newRightLeg = portal.P1 - apexPos;

		// 1. See if moving funnel inwards - RIGHT
		if (Cross(rightLeg, newRightLeg) > 0.0f)
		{
			// 2. See if new line degenerates a line segment - RIGHT
			if (Cross(leftLeg, newRightLeg) < 0.0f) // No crossing
			{
				rightLeg = newRightLeg;
				rightLegIdx = portalIdx;
			}
			else
			{
				// Left leg becomes new apex point
				apexPos += leftLeg;
				apexIdx = leftLegIdx;
				portalIdx = leftLegIdx + 1;
				leftLegIdx = portalIdx;
				rightLegIdx = portalIdx;

				path.emplace_back(apexPos);

				if (portalIdx < amtPortals)
				{
					rightLeg = Portals[rightLegIdx].P1 - apexPos;
					leftLeg = Portals[leftLegIdx].P2 - apexPos;
					continue;
				}
			}
		}

		// --- LEFT CHECK ---
		FVector2D newLeftLeg = portal.P2 - apexPos;

		// 1. See if moving funnel inwards - LEFT
		if (Cross(leftLeg, newLeftLeg) <= 0.0f)
		{
			// 2. See if new line degenerates a line segment - LEFT
			if (Cross(rightLeg, newLeftLeg) > 0.0f) // No crossing
			{
				leftLeg = newLeftLeg;
				leftLegIdx = portalIdx;
			}
			else
			{
				// Right leg becomes new apex point
				apexPos += rightLeg;
				apexIdx = rightLegIdx;
				portalIdx = rightLegIdx + 1;
				rightLegIdx = portalIdx;
				leftLegIdx = portalIdx;

				path.emplace_back(apexPos);

				if (portalIdx < amtPortals)
				{
					rightLeg = Portals[rightLegIdx].P1 - apexPos;
					leftLeg = Portals[leftLegIdx].P2 - apexPos;
					continue;
				}
			}
		}
	}

	// Add last path point
	path.emplace_back(Portals[amtPortals - 1].P1);

	return path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
