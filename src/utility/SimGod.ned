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

package containerVehicles.utility;

simple SimGod
{
    parameters:
        @class(SimGod);
        bool debugMode = default(true);

        int MQTTpublishers = default(5);
        int MQTTsubscribers = default(5);
        int MQTTmsgCount = default(20);
        int MQTTmsgSize = default(2048);
        int MQTTmsgQoS = default(2);
        int MQTTmsgInterval = default(1000);
        int MQTTmsgPretime = default(2000);

        int clientsStartUp = default(5s) @unit("s");
}
