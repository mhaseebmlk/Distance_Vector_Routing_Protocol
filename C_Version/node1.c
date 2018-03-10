#include <stdio.h>
#include <stdbool.h>

extern float clocktime;

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 1 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;

int connectcosts1[4] = { 1,  1,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */


rtinit1() 
{
  printf("[Clocktime %f] rtinit1() called.\n",clocktime);

  // via-based distances
  dt1.costs[0][0] = 999;
  dt1.costs[1][0] = 1;
  dt1.costs[2][0] = 999;
  dt1.costs[3][0] = 999;

  dt1.costs[0][1] = 1;
  dt1.costs[1][1] = 0;
  dt1.costs[2][1] = 1;
  dt1.costs[3][1] = 999;

  dt1.costs[0][2] = 999;
  dt1.costs[1][2] = 1;
  dt1.costs[2][2] = 999;
  dt1.costs[3][2] = 999;

  dt1.costs[0][3] = 999;
  dt1.costs[1][3] = 999;
  dt1.costs[2][3] = 999;
  dt1.costs[3][3] = 999;

  // make rtpkts to send to each of the neighbors
  struct rtpkt rtpkt_To_Node1;
  creatertpkt(&rtpkt_To_Node1,1,0,dt1.costs[1]);
  tolayer2(rtpkt_To_Node1);

  struct rtpkt rtpkt_To_Node2;
  creatertpkt(&rtpkt_To_Node2,1,2,dt1.costs[1]);
  tolayer2(rtpkt_To_Node2); 
}


rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
  printf("[Clocktime %f] rtupdate1() called.\n", clocktime);
  printf("\t[Node1] Received this rtpkt from Node%d\n", rcvdpkt->sourceid);

  bool neighborHasNewCosts = false;
  for (int y=0;y<4;y++) {
    if (dt1.costs[rcvdpkt->sourceid][y] != rcvdpkt->mincost[y]) {
      dt1.costs[rcvdpkt->sourceid][y] = rcvdpkt->mincost[y];
      dt1.costs[y][rcvdpkt->sourceid] = rcvdpkt->mincost[y]; //bidirectional links
      neighborHasNewCosts = true;
    }
  }
  bool dt_Changed = false;
  if (neighborHasNewCosts) {
    for (int y=0;y<4;y++) {
      int cost_x_v, dist_v_y, dist_x_y;
      for (int v=0;v<4;v++) {
        cost_x_v = dt1.costs[1][v];
        dist_v_y = dt1.costs[v][y];
        dist_x_y = cost_x_v + dist_v_y;
        if (dist_x_y < dt1.costs[1][y]) {
          dt1.costs[1][y] = dist_x_y;
          dt_Changed = true;
        }
      }
    }
  }
  if (dt_Changed) {
    printf("\t[Node1] Distance table updated by finding new mincost to a neighbor. Sending the following rtpkts to Node1's neighbors:\n");

    // make rtpkts to send to each of the neighbors
    struct rtpkt rtpkt_To_Node0;
    creatertpkt(&rtpkt_To_Node0,1,0,dt1.costs[1]);
    tolayer2(rtpkt_To_Node0);

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node0.sourceid,rtpkt_To_Node0.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node0.mincost[i]);
    printf("\n");

    struct rtpkt rtpkt_To_Node2;
    creatertpkt(&rtpkt_To_Node2,1,2,dt1.costs[1]);
    tolayer2(rtpkt_To_Node2); 

    printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node2.sourceid,rtpkt_To_Node2.destid);
    for(int i=0;i<4;i++)
      printf("%d ",rtpkt_To_Node2.mincost[i]);
    printf("\n");
  } else if (!dt_Changed && !neighborHasNewCosts) {
    printf("\t[Node1] Distance table not updated.\n");
  } else {
    printf("\t[Node1] Distance table updated.\n");
  }
  printf("[Node1] Printing the distance table. Table represents distance between two nodes. Refer to README for clarifications.\n");
  printdt1(&dt1);
}


printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 1 to 1 */
  
{
  printf("\n[Clocktime %f] linkhandler1() called.\n", clocktime);
  /*
  Problem arises because each of 1's neighbors computes the minimum distances
  and it always comes to be less than dist_x_v + 20 and so the link costs are not 
  updated and old paths from 1 and 0 are still prefered. So, we should manually
  tell other neighbors that the link cost has significantly increased so that they
  can re-compute paths -> one solution to 'count-to-infinity' probelm -> advertise
  'infinite' cost paths 
  */
  // printf("linkid: %d, newcost: %d\n",linkid,newcost);
  dt1.costs[1][linkid] = newcost;

  // make rtpkts to send to each of the neighbors
  printf("\t[Node1] Sending the following rtpkts to Node1's neighbors:\n");
  struct rtpkt rtpkt_To_Node0;
  creatertpkt(&rtpkt_To_Node0,1,0,dt1.costs[1]);
  tolayer2(rtpkt_To_Node0);

  printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node0.sourceid,rtpkt_To_Node0.destid);
  for(int i=0;i<4;i++)
    printf("%d ",rtpkt_To_Node0.mincost[i]);
  printf("\n");

  struct rtpkt rtpkt_To_Node2;
  creatertpkt(&rtpkt_To_Node2,1,2,dt1.costs[1]);
  tolayer2(rtpkt_To_Node2); 

  printf("\tsourceid = %d\n\tdestid = %d\n\tmincost:\n\t",rtpkt_To_Node2.sourceid,rtpkt_To_Node2.destid);
  for(int i=0;i<4;i++)
    printf("%d ",rtpkt_To_Node2.mincost[i]);
  printf("\n");
}

