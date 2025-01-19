# IOT-project-bovine-control

### Hardware Simulation
- **gate1.ino**  
  Simulates a gate or sensor module. It collects or simulates raw data such as cow entries/exits or other sensor readings.  
  Demonstrates how to use Arduino code to handle real-time sensor inputs and outputs.

- **gate2.ino**  
  A second gate module with similar functionality, possibly adding another sensor or scenario.  
  Offers flexibility in testing multiple gate setups or use cases simultaneously.

### ML Code
- **annotated.csv**  
  A sample dataset containing labeled or annotated data on cow behavior or health readings. Used to train and validate the ML model.

- **clustering.ipynb**  
  A Jupyter notebook demonstrating a clustering approach (e.g., K-means) to group cow behavior or health patterns.  
  Useful for exploring the data before deciding on classification or prediction techniques.

- **cow_sickness_model.py**  
  Contains logic for training a model that predicts if a cow might be sick based on sensor readings or other features.  
  Could include functions for data preprocessing, feature engineering, and model definition.

- **evaluation.txt**  
  A text file summarizing model evaluation metrics (accuracy, precision, recall, etc.) or providing insights gleaned from model performance tests.

- **model.py**  
  A script dedicated to building, training, and possibly saving a machine learning model.  
  Often includes hyperparameter tuning or advanced model setup.

- **send_to_FB.py**  
  A script demonstrating how to send predictions or relevant data to Firebase (or another backend service).  
  Illustrates how the IoT pipeline can integrate with cloud services for data storage, alerts, or dashboards.

- **test_cow.py**  
  A simple test harness script that passes sample inputs into the model to verify predictions or classifications.  
  Helps confirm end-to-end functionality of the ML pipeline.

## Usage
1. **Hardware Simulation**  
   - Load `gate1.ino` or `gate2.ino` into your Arduino IDE and upload to the board to simulate gates/sensors.  
   - Adjust sensor or timing variables to suit your environment.

2. **ML Code**  
   - Run `clustering.ipynb` in Jupyter to explore initial data and group patterns.
   - Use `cow_sickness_model.py` (or `model.py`) to train the prediction model.
   - Check results in `evaluation.txt`.
   - Use `test_cow.py` to do a quick check with test data.
   - (Optional) Use `send_to_FB.py` to publish results to a backend (e.g., Firebase).

## Contributing
1. Fork the repository.
2. Create your feature branch: `git checkout -b feature/YourFeature`.
3. Commit your changes: `git commit -am 'Add new feature'`.
4. Push to the branch: `git push origin feature/YourFeature`.
5. Open a pull request.

## License
This project is licensed under the [MIT License](LICENSE). Please see the `LICENSE` file for more information.
