
using System.Collections.Generic;
using Photon.Deterministic;
using UnityEngine;
using System;
namespace Quantum
{
	public partial class AiMapDataSpec : AssetObject
	{
		public enum NodeType
		{
			StaticColliderFloor = 0,

			PlatformFloor = 10,
		}

		public enum MovementType
		{
			None = 0,

			Jump = 10,
			DoubleJump = 11,

			FallDown = 20,
			FallDownJump = 21,

			Ladder = 30,
		}

		/// <summary>
		/// Cost of moving to this node
		/// </summary>
		[Serializable]
		public class MovementCostData
		{
			public MovementType moveType;
			public int cost;
		}

		/// <summary>
		/// node-to-node connection data
		/// </summary>
		[System.Serializable]
		public class NodeLinkData
		{
			public int linkNodeIndex;

			public MovementType movementType;

			public FpRect entryRect;
			public DirectionType entryDirection;

			public FpRect linkRect;
		}

		/// <summary>
		/// the start node to the end node Path Data
		/// </summary>
		[System.Serializable]
		public class NodePathData
		{
			public int goalNodeIndex;
			public int totalCost;
			public List<int> pathNodeList;
		}

		/// <summary>
		/// node data
		/// </summary>
		[System.Serializable]
		public class NodeData
		{
			public int nodeIndex;

			public NodeType nodeType;

			[Space(10)]
			public FpRect nodeRect;

			/// If the character is inside this area, it is considered to be present in this node 
			[Space(10)]
			public FpRect characterPresenceRect;
			/// Exit tolerance distance for presence detection
			public FP presenceExitDistance;

			[Space(10)]
			public List<NodeLinkData> linkList;

			[Space(10)]
			public List<NodePathData> pathList;
		}

		[System.Serializable]
		public class AirNodeLinkData
		{
			public int linkAirNodeIndex;
			public int linkCost;
		}

		[System.Serializable]
		public class AirNodeData
		{
			public int airNodeIndex;
			public FPVector2 airNodePosition;
			public List<AirNodeLinkData> airLinkList;
			public List<NodePathData> airPathList;
		}

		[System.Serializable]
		public class NodeGroupData
		{
			public int groupIndex;
			[Space(10)]
			public FpRect nodeGroupRect;
			[Space(10)]
			public List<NodeData> nodeDataList;
			[Space(10)]
			public List<AirNodeData> airNodeList;
		}

		[Space]
		public List<MovementCostData> costDataList;

		[Space(10)]
		public List<NodeGroupData> nodeGroupList;

		public int GetCost(MovementType type) => costDataList.Find(x => x.moveType == type).cost;

		#region[Node]
		/// <summary>
		/// 현재 위치에 해당 되는 노드 정보를 가져온다.
		/// </summary>
		public bool CurrentAiNode(int preNodeIndex, FPVector2 position, out NodeData nodeData)
		{
			nodeData = null;

			/// 1. 이전 노드에 있는지 여부 확인
			if (GetNodeData(preNodeIndex, out var preNodeData)
						&& preNodeData.characterPresenceRect.InValue(position))
			{
				nodeData = preNodeData;
				return true;
			}

			/// 2. 특정 노드 영역에 있는지 확인
			if (GetNodeDataByPresenceRect(position, out nodeData))
			{
				return true;
			}

			/// 3. 이전 노드범위에서 크게 벗어나지 않았다면 이전노드로 판정
			if (preNodeData != null)
			{
				var nearPoint = preNodeData.characterPresenceRect.NearestPoint(position);
				if (FPVector2.DistanceSquared(nearPoint, position) <= preNodeData.presenceExitDistance * preNodeData.presenceExitDistance)
				{
					nodeData = preNodeData;
					return true;
				}
			}

			return false;
		}

		public bool NextNodeLinkData(int curNodeIndex, int targetNodeIndex, FPVector2 aiPosition, out NodeLinkData linkData, out NodePathData pathData)
		{
			linkData = null;
			pathData = null;

			if (false == GetNodeData(curNodeIndex, out var curNodeData))
				return false;

			pathData = curNodeData.pathList.Find(x => x.goalNodeIndex == targetNodeIndex);
			if (pathData == null || pathData.pathNodeList == null || pathData.pathNodeList.Count == 0)
				return false;

			var nextNodeIndex = pathData.pathNodeList[0];
			var entryDist = FP._0;
			for(int i = 0; i < curNodeData.linkList.Count; i++)
			{
				if (curNodeData.linkList[i].linkNodeIndex != nextNodeIndex)
					continue;

				var tempDist = curNodeData.linkList[i].entryRect.Distance(aiPosition);

				if (linkData == null || tempDist < entryDist)
				{
					linkData = curNodeData.linkList[i];
					entryDist = tempDist;
				}
			}

			return linkData != null;
		}

		public bool GetNodeGroupData(FPVector2 position, out NodeGroupData data)
		{
			data = null;

			foreach(var _data in nodeGroupList)
			{
				if (_data.nodeGroupRect.InValue(position))
				{
					data = _data;
					break;
				}
			}
			return (data != null);
		}
		public bool GetNodeGroupData(int groupIndex, out NodeGroupData data)
		{
			data = null;

			foreach (var _data in nodeGroupList)
			{
				if (_data.groupIndex == groupIndex)
				{
					data = _data;
					break;
				}
			}
			return (data != null);
		}
		public bool GetNodeDataByColliderRect(FPVector2 position, out NodeData nodeData)
		{
			nodeData = null;

			if (false == GetNodeGroupData(position, out var groupData))
				return false;

			var yDist = FP._0;
			foreach (var node in groupData.nodeDataList)
			{
				if (node.nodeRect.InValue(position))
				{
					nodeData = node;
					break;
				}
			}

			return nodeData != null;
		}
		public bool GetNodeDataByPresenceRect(FPVector2 position, out NodeData nodeData)
		{
			nodeData = null;

			if (false == GetNodeGroupData(position, out var groupData))
				return false;

			var yDist = FP._0;
			foreach (var node in groupData.nodeDataList)
			{
				if (node.characterPresenceRect.InValue(position))
				{
					nodeData = node;
					break;
				}
			}

			return nodeData != null;
		}
		public bool GetNodeData(int groupIndex, int nodeIndex, out NodeData nodeData)
		{
			nodeData = null;

			if (false == GetNodeGroupData(groupIndex, out var groupData))
				return false;

			foreach (var node in groupData.nodeDataList)
			{
				if (nodeIndex == node.nodeIndex)
				{
					nodeData = node;
					break;
				}
			}

			return nodeData != null;
		}
		public bool GetNodeData(int nodeIndex, out NodeData nodeData)
		{
			nodeData = null;

			foreach (var groupData in nodeGroupList)
			{
				foreach (var node in groupData.nodeDataList)
				{
					if (nodeIndex == node.nodeIndex)
					{
						nodeData = node;
						break;
					}
				}

				if (nodeData != null)
					break;
			}

			return nodeData != null;
		}
		#endregion

		#region[AirNode]
		public bool GetAirNodeData(NodeGroupData groupData, int airNodeIndex, out AirNodeData airNode)
		{
			airNode = null;

			foreach(var airNodeData in groupData.airNodeList)
			{
				if (airNodeData.airNodeIndex == airNodeIndex)
				{
					airNode = airNodeData;
					break;
				}
			}

			return airNode != null;
		}
		public bool GetAirNodeData(int groupIndex, int airNodeIndex, out AirNodeData airNode)
		{
			airNode = null;

			if (false == GetNodeGroupData(groupIndex, out var groupData))
				return false;

			foreach (var airNodeData in groupData.airNodeList)
			{
				if (airNodeData.airNodeIndex == airNodeIndex)
				{
					airNode = airNodeData;
					break;
				}
			}

			return airNode != null;
		}
		public bool NearAirNode(FPVector2 pos, out AirNodeData nearAirNode)
		{
			nearAirNode = null;

			if (false == GetNodeGroupData(pos, out var groupData))
				return false;

			var nearSqDist = FP._0;
			foreach (var airNode in groupData.airNodeList)
			{
				var tempSqDist = FPVector2.DistanceSquared(airNode.airNodePosition, pos);

				if (nearAirNode == null || tempSqDist < nearSqDist)
				{
					nearAirNode = airNode;
					nearSqDist = tempSqDist;
				}
			}

			return nearAirNode != null;
		}
		public bool NearAirNode(NodeGroupData groupData, FPVector2 pos, out AirNodeData nearAirNode)
		{
			nearAirNode = null;

			var nearSqDist = FP._0;
			foreach (var airNode in groupData.airNodeList)
			{
				var tempSqDist = FPVector2.DistanceSquared(airNode.airNodePosition, pos);

				if (nearAirNode == null || tempSqDist < nearSqDist)
				{
					nearAirNode = airNode;
					nearSqDist = tempSqDist;
				}
			}

			return nearAirNode != null;
		}
		public bool GetTargetAirPathPostion(FPVector2 aiPosition, FPVector2 targetPosition, out FPVector2 movePostion)
		{
			movePostion = targetPosition;

			if (false == GetNodeGroupData(aiPosition, out var groupData))
				return false;

			/// 타겟을 향해 이동하는데 걸리는 부분이 없다면 airNode를 찾기 않고 타겟위치로 바로 이동시킨다.
			if (false == CheckIntersectsLine(groupData, aiPosition, targetPosition))
				return true;

			/// ai와 타겟의 NearAirNode 정보 가져오기
			if (false == NearAirNode(aiPosition, out var aiNearNode)
					|| false == NearAirNode(targetPosition, out var targetNearNode))
				return false;
			
			/// 타겟을 향해 갈수 있는 airNodePath 정보 가져오기
			var pathData = aiNearNode.airPathList.Find(x => x.goalNodeIndex == targetNearNode.airNodeIndex);
			if (pathData == null)
				return false;

			/// 이동가능한 가장 먼 airNode 정보 가져오기
			for(int i = pathData.pathNodeList.Count - 1; i >= 0; i--)
			{
				if (false == GetAirNodeData(groupData, pathData.pathNodeList[i], out var moveAirNode))
					continue;

				/// 이동 가능한지 여부 체크
				if (false == CheckIntersectsLine(groupData, aiPosition, moveAirNode.airNodePosition))
					continue;

				movePostion = moveAirNode.airNodePosition;
				return true;
			}


			return false;
		}
		#endregion

		/// <summary>
		/// 두 점을 잇는 라인이 collider에 의해 막히는지 여부 체크
		/// </summary>
		public bool CheckIntersectsLine(NodeGroupData groupData, FPVector2 pos1, FPVector2 pos2)
		{
			foreach (var nodeData in groupData.nodeDataList)
			{
				if (nodeData.nodeRect.IntersectsLine(pos1, pos2))
					return true;
			}
			return false;
		}
	}
}

