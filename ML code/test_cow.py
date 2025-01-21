import pandas as pd
import open3d as o3d
import numpy as np
import joblib
import os

# Load your CSV once (do this at startup)
df_lookup = pd.read_csv("annotated.csv")

# Columns must match the order of features used in your trained model
FEATURE_COLUMNS = [
    "live weithg",
    "withers height",
    "hip height",
    "chest depth",
    "chest width",
    "ilium width",
    "hip joint width",
    "oblique body length",
    "hip length",
    "heart girth"
]

def get_features_from_csv(cow_id: int):
    """
    Look up the cow's manual measurements from a CSV file
    based on its ID. Return them in a list matching FEATURE_COLUMNS order.
    """
    row = df_lookup[df_lookup["N"] == cow_id]
    if row.empty:
        raise ValueError(f"No data found for cow ID={cow_id} in the CSV.")
    
    # Extract each feature in the correct order
    feats = []
    for col in FEATURE_COLUMNS:
        # col must match the exact CSV header
        val = row.iloc[0][col]
        feats.append(val)
    
    return feats


def extract_ply_features(ply_path):
    """
    Loads a .ply point cloud using Open3D and returns basic geometric features:
    - dx, dy, dz (from axis-aligned bounding box)
    - volume (dx*dy*dz)
    - num_points
    """
    pcd = o3d.io.read_point_cloud(ply_path)
    if not pcd.has_points():
        return (0, 0, 0, 0, 0)  # fallback if empty

    aabb = pcd.get_axis_aligned_bounding_box()
    min_bound = aabb.min_bound
    max_bound = aabb.max_bound
    dims = max_bound - min_bound  # (dx, dy, dz)

    dx, dy, dz = dims[0], dims[1], dims[2]
    volume = dx * dy * dz
    num_points = np.asarray(pcd.points).shape[0]

    return (dx, dy, dz, volume, num_points)

def classify_cow_sickness(cow_id: int, ply_path: str, model_path: str = "cow_sickness_model.pkl"):
    """
    1. Loads the pre-trained Random Forest model from disk.
    2. Looks up the cow's manual measurements by ID from CSV.
    3. Extracts .ply features from the given file.
    4. Creates the final feature vector.
    5. Predicts sick vs. not sick, returning the result.
    """
    # 1) Load model
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Model file not found: {model_path}")
    clf = joblib.load(model_path)

    # 2) Fetch manual features from CSV
    manual_features = get_features_from_csv(cow_id)  # list of 10 measurements

    # 3) Extract ply-based features
    dx, dy, dz, volume, num_points = extract_ply_features(ply_path)

    # 4) Combine into single feature 
    # Make sure to append the ply features in the same order used for training
    # For example, if your training order was:
    # [live weight, withers height, ..., heart girth, dx, dy, dz, volume, num_points]
    feature_vector = manual_features + [dx, dy, dz, volume, num_points]

    # 5) Predict
    prediction = clf.predict([feature_vector])  # e.g. returns [0] or [1]
    pred_label = prediction[0]

    # If your model interprets 1 as 'sick', 0 as 'normal', adjust as needed
    if pred_label == 1:
        return True  # Sick
    else:
        return False # Normal


if __name__ == "__main__":
    # Example usage
    input_cow_id = int(input("Enter Cow ID: "))
    input_ply_path = input("Enter path to cow's .ply file: ")

    is_sick = classify_cow_sickness(input_cow_id, input_ply_path)
    print(f"Cow {input_cow_id} => SICK: {is_sick}")
