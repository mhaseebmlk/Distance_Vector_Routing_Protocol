#include <stdio.h>
#include <stdbool.h>

extern float clocktime;

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
  printf("[Clocktime %f] rtinit2() called.\n",clocktime);

  // via-based distances
  dt2.costs[0][0] = 999;
  dt2.costs[1][0] = 999;
  dt2.costs[2][0] = 3;
  dt2.costs[3][0] = 999;

  dt2.costs[0][1] = 999;
  dt2.costs[1][1] = 999;
  dt2.costs[2][1] = 1;
  dt2.costs[3][1] = 999;

  dt2.costs[0][2] = 3;
  dt2.costs[1][2] = 1;
  dt2.costs[2][2] = 0;
  dt2.costs[3][2] = 2;

  dt2.costs[0][3] = 999;
  dt2.costs[1][3] = 999;
  dt2.costs[2][3] = 2;
  dt2.costs[3][3] = 999;

  // make rtpkts to send to each of the neighbors
  struct rtpkt rtpkt_To_Node0;
  creatertpkt(&rtpkt_To_Node0,2,0,dt2.costs[2]);
  tolayer2(rtpkt_To_Node0);

  struct rtpkt rtpkt_To_Node1;
  creatertpkt(&rtpkt_To_Node1,2,1,dt2.costs[2]);
  tolayer2(rtpkt_To_Node1);

  struct rtpkt rtpkt_To_Node3;
  creatertpkt(&rtpkt_To_Node3,2,3,dt2.costs[2]);
  tolayer2(rtpkt_To_Node3);
}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
  printf("[Clocktime %f] rtupdate2() called.\n", clocktime);
  printf("\t[Node2] Received this rtpkt from Node%d\n", rcvdpkt->sourceid);

  bool neighborHasNewCosts = 0;

  for (int y = 0; y<4;y++){
    if (dt2.costs[rcvdpkt->sourceid][y] != rcvdpkt->mincost[y]) {
      dt2.costs[rcvdpkt->sourceid][y] = rcvdpkt->mincost[y];
      dt2.costs[y][rcvdpkt->sourceid] = rcvdpkt->mincost[y]; //bidirectional links
      neighborHasNewCosts = true;
    }
  }
  bool dt_Changed = false;
  if (neighborHasNewCosts) {
    for (int y=0;y<4;y++) {
      int cost_x_v, dist_v_y, dist_x_y;
      for (int v=0;v<4;v++)  {
        cost_x_v = dt2.costs[2][v];
        dist_v_y = dt2.costs[v][y];
        dist_x_y = cost_x_v+dist_v_y;
        if (dist_x_y < dt2.costs[2][y]) {
          dt2.costs[2][y] = dist_x_y;
          dt_Changed = true;
        }
      }
    }
  }

  if (dt_Changed) {
    printf("\t[Node2] Distance table updated by finding new mincost to a neighbor. Sending the following rtpkts to Node2's neighbors:\n");

    // make rtpkts to send to each of the neighbors
    struct rtpkt rtpkt_To_Node0;
    creatertpkt(&rtpkt_To_Node0,2,0,dt2.costs[2]);
    tolayer2(rtpkt_To_Node0);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node0.sourceid,rtpkt_To_Node0.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node0.mincost[i]);
    printf("\n");

    struct rtpkt rtpkt_To_Node1;
    creatertpkt(&rtpkt_To_Node1,2,1,dt2.costs[2]);
    tolayer2(rtpkt_To_Node1);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node1.sourceid,rtpkt_To_Node1.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node1.mincost[i]);
    printf("\n");

    struct rtpkt rtpkt_To_Node3;
    creatertpkt(&rtpkt_To_Node3,2,3,dt2.costs[2]);
    tolayer2(rtpkt_To_Node3); 

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node3.sourceid,rtpkt_To_Node3.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node3.mincost[i]);
    printf("\n");
  } else if (!dt_Changed && !neighborHasNewCosts) {
    printf("\t[Node2] Distance table not updated.\n");
  } else {
    printf("\t[Node2] Distance table updated.\n");
  }
  printf("[Node2] Printing the distance table. Table represents distance between two nodes. Refer to README for clarifications.\n");
  printdt2(&dt2);
}


printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
   dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
   dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
   dtptr->costs[3][1],dtptr->costs[3][3]);
}






