using System.Collections.Generic;
using UnityEngine;
using System;
using Sirenix.OdinInspector;
using Quantum;
using Photon.Deterministic;

public class AiMapDataBake : MonoBehaviour
{
    public AiMapDataSpec aiMapDataSpec;

    [Space(2), Title("Node 그룹 리스트")]
    public List<NodeGroupObject> nodeGroupObjList;

    [Space(2), Title("GUI")]
    public bool ShowNodeLink = false;
    public bool ShowNodePath = false;
    public Color NodePathColor;

#if UNITY_EDITOR
    [Button(50)]
    private void BakeAiMapData()
    {
        if (null == aiMapDataSpec || null == nodeGroupObjList)
            return;

        aiMapDataSpec.nodeGroupList = new List<AiMapDataSpec.NodeGroupData>();

        BakeObjectData();

        BakePathData();

        UnityEditor.EditorUtility.SetDirty(aiMapDataSpec);


        void BakeObjectData()
		{
            foreach(var groupObj in nodeGroupObjList)
			{
                var groupData = new AiMapDataSpec.NodeGroupData()
				{
                    groupIndex = groupObj.groupIndex,
                    nodeGroupRect = ConvertFpRect(groupObj.groupRect),
                    nodeDataList = new List<AiMapDataSpec.NodeData>(),
                };

                /// 노드 데이터 추가
                foreach (var nodeObj in groupObj.nodeObjList)
                {
                    var nodeData = new AiMapDataSpec.NodeData()
                    {
                        nodeIndex = nodeObj.nodeIndex,
                        nodeType = nodeObj.nodeType,
                        nodeRect = ConvertFpRect(nodeObj.colliderRect),
                        characterPresenceRect = ConvertFpRect(nodeObj.presenceRect),
                        presenceExitDistance = nodeObj.presenceExitDistance.ToFP(),
                        linkList = new List<AiMapDataSpec.NodeLinkData>(),
                        pathList = new List<AiMapDataSpec.NodePathData>()
                    };

                    foreach (var linkObj in nodeObj.nodeLinkObjList)
                    {
                        var linkData = new AiMapDataSpec.NodeLinkData()
                        {
                            linkNodeIndex = linkObj.goalNode.nodeIndex,
                            movementType = linkObj.moveType,
                            entryRect = ConvertFpRect(linkObj.entryRect),
                            entryDirection = linkObj.entryDirectionType,
                            linkRect = ConvertFpRect(linkObj.goalRect),
                        };

                        nodeData.linkList.Add(linkData);
                    }

                    groupData.nodeDataList.Add(nodeData);
                }

                /// air 노드 데이터 추가
                foreach (var airNodeObj in groupObj.airPathNodeList)
                {
                    /// 노드 데이터 설정
                    var airNodeData = new AiMapDataSpec.AirNodeData()
                    {
                        airNodeIndex = airNodeObj.airPathNodeIndex,
                        airNodePosition = airNodeObj.transform.position.ToFPVector2(),
                        airLinkList = new List<AiMapDataSpec.AirNodeLinkData>(),
                        airPathList = new List<AiMapDataSpec.NodePathData>()
                    };

                    /// 노드 링크 데이터 설정
                    foreach (var airLinkObj in airNodeObj.linkAirPathNodeList)
                    {
                        var airLinkData = new AiMapDataSpec.AirNodeLinkData()
                        {
                            linkAirNodeIndex = airLinkObj.linkAirPathNode.airPathNodeIndex,
                            linkCost = airLinkObj.linkCost,
                        };

                        airNodeData.airLinkList.Add(airLinkData);
                    }

                    groupData.airNodeList.Add(airNodeData);
                }

                aiMapDataSpec.nodeGroupList.Add(groupData);
            }
        }

        void BakePathData()
		{
            foreach(var groupData in aiMapDataSpec.nodeGroupList)
			{
                /// 해당 노드에서 같은 그룹의 모든 노드로 가는 최단경로 값 Data 설정
                foreach (var nodeData in groupData.nodeDataList)
                {
                    nodeData.pathList = new List<AiMapDataSpec.NodePathData>();

                    foreach (var targetNode in groupData.nodeDataList)
                    {
                        var pathData = new AiMapDataSpec.NodePathData()
                        {
                            goalNodeIndex = targetNode.nodeIndex,
                            totalCost = (nodeData.nodeIndex == targetNode.nodeIndex) ? 0 : int.MaxValue,
                            pathNodeList = new List<int>(),
                        };
                        nodeData.pathList.Add(pathData);
                    }


                    /// 해당 노드를 시작 노드로 해서 모든 노드로 갈수 있는 경로 정보를 저장
                    /// nodeData.pathList : 경로 저장 변수
                    /// closeNodeList : 체크가 완료된 노드 저장 리스트
                    var closeNodeList = new List<int>();
                    UpdateNodeLinkPath(groupData.groupIndex, nodeData.nodeIndex, ref nodeData.pathList, ref closeNodeList);
                }

                /// 해당 air 노드에서 같은 그룹의 모든 air 노드로 가는 최단경로 값 Data 설정
                foreach (var airNodeData in groupData.airNodeList)
                {
                    airNodeData.airPathList = new List<AiMapDataSpec.NodePathData>();

                    foreach (var targetAirNode in groupData.airNodeList)
                    {
                        var pathData = new AiMapDataSpec.NodePathData()
                        {
                            goalNodeIndex = targetAirNode.airNodeIndex,
                            totalCost = (airNodeData.airNodeIndex == targetAirNode.airNodeIndex) ? 0 : int.MaxValue,
                            pathNodeList = new List<int>(),
                        };
                        airNodeData.airPathList.Add(pathData);
                    }


                    /// 해당 노드를 시작 노드로 해서 모든 노드로 갈수 있는 경로 정보를 저장
                    /// nodeData.pathList : 경로 저장 변수
                    /// closeAirNodeList : 체크가 완료된 노드 저장 리스트
                    var closeAirNodeList = new List<int>();
                    UpdateAirNodeLinkPath(groupData.groupIndex, airNodeData.airNodeIndex, ref airNodeData.airPathList, ref closeAirNodeList);
                }
            }
            
            /// 해당 노드에서 연결되어있는 노드들의 경로 값 업데이트
            void UpdateNodeLinkPath(int groupIndex, int currentNodeIndex, ref List<AiMapDataSpec.NodePathData> _pathList, ref List<int> _closeNodeList)
            {
                if (_closeNodeList.Contains(currentNodeIndex))
                    return;
                else
                    _closeNodeList.Add(currentNodeIndex);

                if (false == aiMapDataSpec.GetNodeData(groupIndex, currentNodeIndex, out var currentNodeData))
				{
                    LOG.E($"UpdateNodeLinkPath error, groupIndex({groupIndex}) currentNodeIndex({currentNodeIndex})");
                    return;
				}

                /// 현재 노드까지의 경로 정보를 가져온다.
                var currentPathData = _pathList.Find(x => x.goalNodeIndex == currentNodeIndex);

                /// 현재 노드와 연결된 노드들의 경로 비용을 업데이트 한다.
                foreach(var linkData in currentNodeData.linkList)
				{
                    var currentMoveCost = currentPathData.totalCost + aiMapDataSpec.GetCost(linkData.movementType);

                    /// 기존 비용보다 저렴하면 재 설정
                    var linkNodePathData = _pathList.Find(x => x.goalNodeIndex == linkData.linkNodeIndex);
                    if (linkNodePathData.totalCost == int.MaxValue 
                                || linkNodePathData.totalCost > currentMoveCost)
					{
                        linkNodePathData.totalCost = currentMoveCost;
                        linkNodePathData.pathNodeList.Clear();
                        linkNodePathData.pathNodeList.AddRange(currentPathData.pathNodeList);
                        linkNodePathData.pathNodeList.Add(linkData.linkNodeIndex);
                    }
                }

                /// 연결된 노드들 경로 업데이트
                foreach(var linkData in currentNodeData.linkList)
				{
                    UpdateNodeLinkPath(groupIndex, linkData.linkNodeIndex, ref _pathList, ref _closeNodeList);
                }
            }
            void UpdateAirNodeLinkPath(int groupIndex, int currentAirNodeIndex, ref List<AiMapDataSpec.NodePathData> _pathList, ref List<int> _closeAirNodeList)
            {
                if (_closeAirNodeList.Contains(currentAirNodeIndex))
                    return;
                else
                    _closeAirNodeList.Add(currentAirNodeIndex);

                if (false == aiMapDataSpec.GetAirNodeData(groupIndex, currentAirNodeIndex, out var currentNodeData))
                {
                    LOG.E($"UpdateNodeLinkPath error, groupIndex({groupIndex}) currentAirNodeIndex({currentAirNodeIndex})");
                    return;
                }

                /// 현재 노드까지의 경로 정보를 가져온다.
                var currentPathData = _pathList.Find(x => x.goalNodeIndex == currentAirNodeIndex);

                /// 현재 노드와 연결된 노드들의 경로 비용을 업데이트 한다.
                foreach (var airLinkData in currentNodeData.airLinkList)
                {
                    var currentMoveCost = currentPathData.totalCost + airLinkData.linkCost;

                    /// 기존 비용보다 저렴하면 재 설정
                    var linkAirNodePathData = _pathList.Find(x => x.goalNodeIndex == airLinkData.linkAirNodeIndex);
                    if (linkAirNodePathData.totalCost == int.MaxValue
                                || linkAirNodePathData.totalCost > currentMoveCost)
                    {
                        linkAirNodePathData.totalCost = currentMoveCost;
                        linkAirNodePathData.pathNodeList.Clear();
                        linkAirNodePathData.pathNodeList.AddRange(currentPathData.pathNodeList);
                        linkAirNodePathData.pathNodeList.Add(airLinkData.linkAirNodeIndex);
                    }
                }

                /// 연결된 노드들 경로 업데이트
                foreach (var linkData in currentNodeData.airLinkList)
                {
                    UpdateNodeLinkPath(groupIndex, linkData.linkAirNodeIndex, ref _pathList, ref _closeAirNodeList);
                }
            }
        }
    }

    void OnDrawGizmos()
    {
        SetDrawNodeObjectData();

        DrawNodePath();

        void SetDrawNodeObjectData()
		{
            foreach(var groupObj in nodeGroupObjList)
			{
                if (groupObj.IsNullOrMissing())
                    continue;

                foreach (var nodeObj in groupObj.nodeObjList)
                {
                    if (nodeObj.IsNullOrMissing())
                        continue;

                    foreach (var nodeLinkObj in nodeObj.nodeLinkObjList)
                    {
                        if (nodeLinkObj.IsNullOrMissing())
                            continue;

                        nodeLinkObj.DrawNodeLinkData = ShowNodeLink;
                    }
                }
            }
		}

        void DrawNodePath()
		{
            if (false == ShowNodePath
                    || null == aiMapDataSpec)
                return;
            var selectNode = (NodeObject)null;

            foreach(var groupObj in nodeGroupObjList)
            {
                selectNode = groupObj.nodeObjList.Find(x => x.SelectNode);
                if (null != selectNode)
                    break;
            }

            if (null == selectNode)
                return;
            if (false == aiMapDataSpec.GetNodeData(selectNode.nodeIndex, out var selectNodeData))
                return;

            Gizmos.color = NodePathColor;

            var startPoint = selectNodeData.nodeRect.WorldCenter;

            foreach (var nodeData in selectNodeData.pathList)
            {
                for (int i = 0; i < nodeData.pathNodeList.Count; i++)
                {
                    if (false == aiMapDataSpec.GetNodeData(nodeData.pathNodeList[i], out var pathNodeData))
                        continue;

                    if (i == 0)
                    {
                        Gizmos.DrawLine(startPoint.ToUnityVector2(), pathNodeData.nodeRect.NearestPoint(startPoint).ToUnityVector2());
                    }
                    else
                    {
                        if (false == aiMapDataSpec.GetNodeData(nodeData.pathNodeList[i - 1], out var preNodeData))
                            continue;

                        var prePoint = preNodeData.nodeRect.WorldCenter;
                        Gizmos.DrawLine(prePoint.ToUnityVector2(), pathNodeData.nodeRect.NearestPoint(prePoint).ToUnityVector2());
                    }
                }
            }

            Gizmos.DrawCube(startPoint.ToUnityVector2(), selectNodeData.nodeRect.Size.ToUnityVector2() + Vector2.one);
        }
    }
#endif

    public static FpRect ConvertFpRect(Renderer renderer) => new FpRect(renderer.bounds.min.ToFPVector2(), renderer.bounds.max.ToFPVector2());
}
