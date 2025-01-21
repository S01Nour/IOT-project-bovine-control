
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix
import joblib  # for exporting model

# 1) LOAD CSV DATA
csv_path = "annotated.csv"  # <-- change to your actual CSV file
df = pd.read_csv(csv_path)

# ---------------------------------------------------------
# 2) SELECT FEATURES & TARGET
#    Adjust feature_cols based on columns in your CSV.
#    We'll assume "anomaly_label" is the target (0=normal, 1=sick) or vice versa.
# ---------------------------------------------------------
feature_cols = [
    "live weithg",       
    "withers height",
    "hip height",
    "chest depth",
    "chest width",
    "ilium width",
    "hip joint width",
    "oblique body length",
    "hip length",
    "heart girth",
    "dx",
    "dy",
    "dz",
    "volume",
    "num_points"
]

target_col = "anomaly" 

# Drop any rows missing necessary columns
df = df.dropna(subset=feature_cols + [target_col])

X = df[feature_cols]
y = df[target_col]

# ---------------------------------------------------------
# 3) TRAIN/TEST SPLIT
# ---------------------------------------------------------
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# ---------------------------------------------------------
# 4) TRAIN RANDOM FOREST
# ---------------------------------------------------------
clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(X_train, y_train)

# ---------------------------------------------------------
# 5) EVALUATE
# ---------------------------------------------------------
y_pred = clf.predict(X_test)
print("Confusion Matrix:")
print(confusion_matrix(y_test, y_pred))
print("\nClassification Report:")
print(classification_report(y_test, y_pred))

# ---------------------------------------------------------
# 6) EXPORT MODEL
# ---------------------------------------------------------
model_filename = "cow_sickness_model.pkl"
joblib.dump(clf, model_filename)
print(f"Model exported to {model_filename}")
