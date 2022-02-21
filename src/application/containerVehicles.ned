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

package containerVehicles.application;

import org.car2x.veins.base.modules.IBaseApplLayer;

simple containerVehicles like IBaseApplLayer
{
    parameters:
        @class(containerVehicles);
        int headerLength @unit("bit");
        int beaconPacketLength = default(10byte) @unit("byte"); //packet length of the application
        int maxPacketLength = default(1500byte) @unit("byte"); //packet length of the application

        int migrationStart = default(10s) @unit(s); //in this time the vehicle has speed 0 m/s

        int migrationType = default(0); //0 for cold migration, 1 for pre-migration, 2 for post-selective-migration

        double preMigrationSize = default(102400byte) @unit("byte");

		double beaconIntervalDefault = default(1s) @unit(s); //the interval between 2 beacon messages
	    double preMigrationInterval = default(5s) @unit(s); //the interval for a new iteration of the pre-migration
		bool avoidBeaconSynchronization = default(true);
		bool sendBeacons = default(true);
		bool clusterHead = default(false);

    gates:
        input lowerLayerIn; // from mac layer
        output lowerLayerOut; // to mac layer
        input lowerControlIn;
        output lowerControlOut;
        
}
