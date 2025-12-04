import xml.etree.ElementTree as ET
import math
import os


def convert_osm_to_edgelist(osm_file, output_file):

    def calculate_distance(lon1, lat1, lon2, lat2):
        R = 6371000  # Earth radius in meters
        lat1, lon1, lat2, lon2 = map(math.radians, [lat1, lon1, lat2, lon2])
        dlat = lat2 - lat1
        dlon = lon2 - lon1
        a = math.sin(dlat / 2) ** 2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2) ** 2
        return R * 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))

    tree = ET.parse(osm_file)
    root = tree.getroot()

    nodes = {}
    for node in root.findall('node'):
        node_id = node.get('id')
        lat = float(node.get('lat'))
        lon = float(node.get('lon'))
        nodes[node_id] = (lat, lon)

    print(f"Found {len(nodes)} nodes")

    # Process roads and create edges
    edges = []
    roads_processed = 0

    for way in root.findall('way'):
        node_ids = []
        road_tags = {}

        # Get all nodes and tags in this way
        for child in way:
            if child.tag == 'nd':
                node_ids.append(child.get('ref'))
            elif child.tag == 'tag':
                road_tags[child.get('k')] = child.get('v')

        if 'highway' in road_tags and len(node_ids) > 1:
            roads_processed += 1

            is_oneway = road_tags.get('oneway') in ('yes', '1', 'true')
            is_reverse_oneway = road_tags.get('oneway') == '-1'

            for i in range(len(node_ids) - 1):
                if is_reverse_oneway:
                    source, target = node_ids[i + 1], node_ids[i]
                else:
                    source, target = node_ids[i], node_ids[i + 1]

                if source in nodes and target in nodes:
                    lat1, lon1 = nodes[source]
                    lat2, lon2 = nodes[target]
                    distance = calculate_distance(lon1, lat1, lon2, lat2)

                    edges.append((source, target, distance))

                    if not is_oneway and not is_reverse_oneway:
                        edges.append((target, source, distance))

    print(f"Processed {roads_processed} roads")
    print(f"Created {len(edges)} edges")
    print(f"Saving to {output_file}...")
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("source,target,weight\n")
        for source, target, weight in edges:
            f.write(f"{source},{target},{weight:.2f}\n")
    return len(edges)


def main():
    osm_file = "map (6).osm"
    output_file = "graph_edges.csv"

    if not os.path.exists(osm_file):
        print(f"ERROR: File '{osm_file}' not found!")
        return

    try:
        edge_count = convert_osm_to_edgelist(osm_file, output_file)
        print(f"Output file: {output_file}")

    except Exception as e:
        print(f"ERROR: {e}")


if __name__ == "__main__":
    main()