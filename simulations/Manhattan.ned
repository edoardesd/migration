//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package containerVehicles.simulations;

import containerVehicles.utility.SimGod;

import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerForker;

import org.car2x.veins.modules.obstacle.ObstacleControl;

import org.car2x.veins.modules.world.annotations.AnnotationManager;
import org.car2x.veins.nodes.Car;
import org.car2x.veins.nodes.RSU;


network scenario
{
    submodules:
        simGod: SimGod;

        annotations: AnnotationManager {
            @display("p=81,28");
        }
        world: BaseWorldUtility {
            @display("p=246,28");
        }
        connectionManager: ConnectionManager {
            @display("p=75,125");
        }
        manager: TraCIScenarioManagerForker {
            @display("p=258,125");
        }
//        obstacles: ObstacleControl {
//            @display("p=398,130");
//        }
        node[0]: Car {
            @display("p=245,240");
        }


        
    connections allowunconnected:
}

