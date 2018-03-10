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
} dt0;


/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
  printf("[Clocktime %f] rtinit0() called.\n",clocktime);

  // via-based distances
  dt0.costs[0][0] = 0;
  dt0.costs[1][0] = 1;
  dt0.costs[2][0] = 3;
  dt0.costs[3][0] = 7;

  dt0.costs[0][1] = 1;
  dt0.costs[1][1] = 999;
  dt0.costs[2][1] = 999;
  dt0.costs[3][1] = 999;

  dt0.costs[0][2] = 3;
  dt0.costs[1][2] = 999;
  dt0.costs[2][2] = 999;
  dt0.costs[3][2] = 999;

  dt0.costs[0][3] = 7;
  dt0.costs[1][3] = 999;
  dt0.costs[2][3] = 999;
  dt0.costs[3][3] = 999;

  // make rtpkts to send to each of the neighbors
  struct rtpkt rtpkt_To_Node1;
  creatertpkt(&rtpkt_To_Node1,0,1,dt0.costs[0]);
  tolayer2(rtpkt_To_Node1);

  struct rtpkt rtpkt_To_Node2;
  creatertpkt(&rtpkt_To_Node2,0,2,dt0.costs[0]);
  tolayer2(rtpkt_To_Node2);

  struct rtpkt rtpkt_To_Node3;
  creatertpkt(&rtpkt_To_Node3,0,3,dt0.costs[0]);
  tolayer2(rtpkt_To_Node3);
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("[Clocktime %f] rtupdate0() called.\n", clocktime);
  printf("\t[Node0] Received this rtpkt from Node%d\n", rcvdpkt->sourceid);

  // when a neighbor node v sends its DV in the rcvpkt, we check if it contains new information
  bool neighborHasNewCosts = false;
  for (int y=0;y<4;y++){
    // if it does, we update our record of the DV for this neighboring node
    if (dt0.costs[rcvdpkt->sourceid][y] != rcvdpkt->mincost[y]) {
      dt0.costs[rcvdpkt->sourceid][y] = rcvdpkt->mincost[y];
      dt0.costs[y][rcvdpkt->sourceid] = rcvdpkt->mincost[y]; //bidirectional links
      neighborHasNewCosts = true;
    }
  }
  bool dt_Changed = false;
  if (neighborHasNewCosts) {
    // we then re-compute the minimum distances to all nodes y through each of our neighbors v
    for (int y=0;y<4;y++) {
      int cost_x_v, dist_v_y, dist_x_y;
      // assume that the minimum distance to the node y from node 0 is dt0[0][y]
      for (int v=0;v<4;v++) {
        cost_x_v = dt0.costs[0][v];
        dist_v_y = dt0.costs[v][y];
        dist_x_y = cost_x_v + dist_v_y;
        if (dist_x_y < dt0.costs[0][y]) {
          dt0.costs[0][y] = dist_x_y;
          dt_Changed = true;
        }
      }
    }
  }
  
  // if the costs change, we tell our neighbors about it
  if (dt_Changed) {
    printf("\t[Node0] Distance table updated by finding new mincost to a neighbor. Sending the following rtpkts to Node0's neighbors:\n");

    // make rtpkts to send to each of the neighbors
    struct rtpkt rtpkt_To_Node1;
    creatertpkt(&rtpkt_To_Node1,0,1,dt0.costs[0]);
    tolayer2(rtpkt_To_Node1);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node1.sourceid,rtpkt_To_Node1.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node1.mincost[i]);
    printf("\n");

    struct rtpkt rtpkt_To_Node2;
    creatertpkt(&rtpkt_To_Node2,0,2,dt0.costs[0]);
    tolayer2(rtpkt_To_Node2);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node2.sourceid,rtpkt_To_Node2.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node2.mincost[i]);
    printf("\n");

    struct rtpkt rtpkt_To_Node3;
    creatertpkt(&rtpkt_To_Node3,0,3,dt0.costs[0]);
    tolayer2(rtpkt_To_Node3);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node3.sourceid,rtpkt_To_Node3.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node3.mincost[i]);
    printf("\n");

  } else if (!dt_Changed && !neighborHasNewCosts) {
    printf("\t[Node0] Distance table not updated.\n");
  } else {
    printf("\t[Node0] Distance table updated.\n");
  }
  printf("[Node0] Printing the distance table. Table represents distance between two nodes. Refer to README for clarifications.\n");
  printdt0(&dt0);
}


printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
   dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
   dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
   dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
  
{
  printf("\n[Clocktime %f] linkhandler0() called.\n", clocktime);
  /*
  Problem arises because each of 0's neighbors computes the minimum distances
  and it always comes to be less than dist_x_v + 20 and so the link costs are not 
  updated and old paths from 1 and 0 are still prefered. So, we should manually
  tell other neighbors that the link cost has significantly increased so that they
  can re-compute paths -> one solution to 'count-to-infinity' probelm -> advertise
  'infinite' cost paths 
  */
  dt0.costs[0][linkid] = newcost;

  // make rtpkts to send to each of the neighbors
  printf("\t[Node0] Sending the following rtpkts to Node1's neighbors:\n");
  struct rtpkt rtpkt_To_Node1;
  creatertpkt(&rtpkt_To_Node1,0,1,dt0.costs[0]);
  tolayer2(rtpkt_To_Node1);

  printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node1.sourceid,rtpkt_To_Node1.destid);
  for(int i=0;i<4;i++)
    printf("%d ",rtpkt_To_Node1.mincost[i]);
  printf("\n");

  struct rtpkt rtpkt_To_Node2;
  creatertpkt(&rtpkt_To_Node2,0,2,dt0.costs[0]);
  tolayer2(rtpkt_To_Node2);

  printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node2.sourceid,rtpkt_To_Node2.destid);
  for(int i=0;i<4;i++)
    printf("%d ",rtpkt_To_Node2.mincost[i]);
  printf("\n");

  struct rtpkt rtpkt_To_Node3;
  creatertpkt(&rtpkt_To_Node3,0,3,dt0.costs[0]);
  tolayer2(rtpkt_To_Node3);

  printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node3.sourceid,rtpkt_To_Node3.destid);
  for(int i=0;i<4;i++)
    printf("%d ",rtpkt_To_Node3.mincost[i]);
  printf("\n");
}
