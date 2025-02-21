import numpy as np
import matplotlib.pyplot as plt
import random
import pandas as pd
import math
from scipy.stats import norm
import os
 

# Phoenix Dataset: https://drive.google.com/drive/folders/1FMLA77VUjdAaS0GXRjvVg4S3rg4IANDz 
 

# Define dataset file paths

datasets = {
    "df9094": "TGSIM/I90_I94_Moving_Trajectories.csv",
    "df294l1": "TGSIM/I294_L1_Trajectories.csv",



    # "dfphoenixh1a3_run1": "TGSIM/H1A3_run1_X_increase.csv",    # BAD DATA
    # "dfphoenixh1a3_run3": "TGSIM/H1A3_run3_Y_decrease.csv",
    # "dfphoenixh1a3_run4": "TGSIM/H1A3_run4_X_decrease.csv",   # Don't use this
    # "dfphoenixh1a3_run5": "TGSIM/H1A3_run5_Y_decrease.csv",   # Goes in the opposite direction 
    "dfphoenixh1a3_run6": "TGSIM/H1A3_run6_Y_increase.csv",    # GOOD DATA TO USE (SWAPPED)
    # "dfphoenixh1a3_run7": "TGSIM/H1A3_run7_X_decrease.csv",  # BAD DATA 
    # "dfphoenixh1a3_run8NS": "TGSIM/H1A3_run8_X_NS_increase.csv",  # BAD DATA
    "dfphoenixh1a3_run8EW": "TGSIM/H1A3_run8_Y_EW_increase.csv", # Has lane-changing so check later
    "dfphoenixh1a3_run9NS": "TGSIM/H1A3_run9_X_NS_increase.csv", # GOOD DATA TO USE (NO SWAPPING)
    "dfphoenixh1a3_run9ES": "TGSIM/H1A3_run9_Y_EW_increase.csv", # OK DATA TO USE 


    # "dfphoenixh2a5_run1": "TGSIM/H2A5_run1_Y_W_decrease.csv",
    # "dfphoenixh2a5_run2": "TGSIM/H2A5_run2_X_S_increase.csv", # BAD DATA NO AV DATA AT ALL!            
    # "dfphoenixh2a5_run3": "TGSIM/H2A5_run3_Y_W_decrease.csv",
    # "dfphoenixh2a5_run4": "TGSIM/H2A5_run4_Y_W_decrease.csv",
    # "dfphoenixh2a5_run5": "TGSIM/H2A5_run5_X_N_decrease.csv",
    # "dfphoenixh2a5_run6": "TGSIM/H2A5_run6_Y_W_decrease.csv",
}



# Define groups to reference the correct separate lists
groups = {
    "df9094": ["I9094_A"],
    "df294l1": ["I294l1_A"],


    # "dfphoenixh1a3_run1": ["Phoenix_H1A3_run1"],   
    # "dfphoenixh1a3_run3": ["Phoenix_H1A3_run3"],
    # "dfphoenixh1a3_run4": ["Phoenix_H1A3_run4"],
    # "dfphoenixh1a3_run5": ["Phoenix_H1A3_run5"],
    "dfphoenixh1a3_run6": ["Phoenix_H1A3_run6"],
    # "dfphoenixh1a3_run7": ["Phoenix_H1A3_run7"],
    # "dfphoenixh1a3_run8NS": ["Phoenix_H1A3_run8NS"],
    "dfphoenixh1a3_run8EW": ["Phoenix_H1A3_run8EW"],
    "dfphoenixh1a3_run9NS": ["Phoenix_H1A3_run9NS"],
    "dfphoenixh1a3_run9ES": ["Phoenix_H1A3_run9ES"],


    # "dfphoenixh2a5_run1": ["Phoenix_H2A5_run1"],
    # "dfphoenixh2a5_run2": ["Phoenix_H2A5_run2"],
    # "dfphoenixh2a5_run3": ["Phoenix_H2A5_run3"],
    # "dfphoenixh2a5_run4": ["Phoenix_H2A5_run4"],
    # "dfphoenixh2a5_run5": ["Phoenix_H2A5_run5"],
    # "dfphoenixh2a5_run6": ["Phoenix_H2A5_run6"], 
}



# Define separate arrays for each dataset
I9094_A = []
I294l1_A = []

Phoenix_H2A5_run1 = []
Phoenix_H2A5_run2 = []
Phoenix_H2A5_run3 = []
Phoenix_H2A5_run4 = []
Phoenix_H2A5_run5 = []
Phoenix_H2A5_run6 = []

Phoenix_H1A3_run1 = []
Phoenix_H1A3_run3 = []
Phoenix_H1A3_run4 = []
Phoenix_H1A3_run5 = []
Phoenix_H1A3_run6 = []
Phoenix_H1A3_run7 = []
Phoenix_H1A3_run8NS = []
Phoenix_H1A3_run8EW = []
Phoenix_H1A3_run9NS = []
Phoenix_H1A3_run9ES = []


############### READ AND ITERATE THROUGH EACH DATA AND STORE THE ACC TYPE ID AND RUN INDEX ################################# 
for data_key, data_path in datasets.items():
    temp_df = pd.read_csv(data_path)

    if data_key == 'df9094':
        temp_df_av = temp_df[temp_df['av'] == 'yes'] 
        I9094_A = temp_df_av[['id', 'run_index']].drop_duplicates().values.tolist()



    elif data_key == 'df294l1':
        temp_df['acc'] = temp_df['acc'].str.lower()
        temp_df_av = temp_df[temp_df['acc'] == 'yes']

        I294l1_A = temp_df_av[['id', 'run_index']].drop_duplicates().values.tolist()


    else:
        temp_df_av = temp_df[temp_df['vehicle-type'] == 'A'].drop_duplicates()
        temp_df_id = temp_df_av['id'].unique()

        dataset_map = {
            "dfphoenixh2a5_run1": Phoenix_H2A5_run1,
            "dfphoenixh2a5_run2": Phoenix_H2A5_run2,
            "dfphoenixh2a5_run3": Phoenix_H2A5_run3,
            "dfphoenixh2a5_run4": Phoenix_H2A5_run4,
            "dfphoenixh2a5_run5": Phoenix_H2A5_run5,
            "dfphoenixh2a5_run6": Phoenix_H2A5_run6,
            "dfphoenixh1a3_run1": Phoenix_H1A3_run1,
            "dfphoenixh1a3_run3": Phoenix_H1A3_run3,
            "dfphoenixh1a3_run4": Phoenix_H1A3_run4,
            "dfphoenixh1a3_run5": Phoenix_H1A3_run5,
            "dfphoenixh1a3_run6": Phoenix_H1A3_run6,
            "dfphoenixh1a3_run7": Phoenix_H1A3_run7,
            "dfphoenixh1a3_run8NS": Phoenix_H1A3_run8NS,
            "dfphoenixh1a3_run8EW": Phoenix_H1A3_run8EW,
            "dfphoenixh1a3_run9NS": Phoenix_H1A3_run9NS,
            "dfphoenixh1a3_run9ES": Phoenix_H1A3_run9ES,
        }

        if data_key in dataset_map:
            for id_val in temp_df_id:
                dataset_map[data_key].append([id_val, 1])

 

print('I90/94')
print(I9094_A)

print('I294l1')
print(I294l1_A)



####################### SIMULATION PARAMETERS ##########################################################
population_size = 100  # Keep as is (sufficient for convergence)
num_generations = 100  # Increase for better tuning
mutation_rate = 0.1  # Reduce mutation rate for better stability
 
 
th_min = 1.5
th_max = 2.0
dmin_min = 2.5
dmin_max = 3.5
lamb_min = 0.00001
lamb_max = 0.0001
most_leading_leader_id = None
#################################################################################################################
 


def find_leader_data(df, follower_id, run_index):
    global most_leading_leader_id
    
    follower_data = df[(df['id'] == follower_id) & (df['run_index'] == run_index)]
    
    print(f"Finding leader for Follower ID {follower_id} and Run Index {run_index}. Rows found: {len(follower_data)}")
    
    leader_data_dict = {}
    
    for index, row in follower_data.iterrows():
        time = row['time']
        follower_x = row[pos]
        follower_lane = row['lane_kf']
        current_run_index = row['run_index']  # Rename to avoid overwriting
        
        leader_data = df[(df['id'] != follower_id) & 
                         (df['time'] == time) & 
                         (df['lane_kf'] == follower_lane) & 
                         (df[pos] > follower_x) & 
                         (df['run_index'] == current_run_index)]
        
        if not leader_data.empty:
            nearest_leader_row = leader_data.loc[leader_data[pos].sub(follower_x).abs().idxmin()]
            
            leader_id = nearest_leader_row['id']
            leader_x_val = nearest_leader_row[pos]
            leader_speed_val = nearest_leader_row['speed_kf']

            if leader_id not in leader_data_dict:
                leader_data_dict[leader_id] = {'time': [], 'x_val': [], 'speed_val': []}

            leader_data_dict[leader_id]['time'].append(time)
            leader_data_dict[leader_id]['x_val'].append(leader_x_val)
            leader_data_dict[leader_id]['speed_val'].append(leader_speed_val)

    if leader_data_dict:
        most_leading_leader_id = max(leader_data_dict, key=lambda x: len(leader_data_dict[x]['time'])) 
        leader_data = leader_data_dict[most_leading_leader_id]
        leader_df = pd.DataFrame({'id': most_leading_leader_id,
                                   'time': leader_data['time'],
                                   pos: leader_data['x_val'],
                                   'speed_kf': leader_data['speed_val'],
                                   'run_index': run_index})  # Ensure this is not overwritten
    else:
        leader_df = pd.DataFrame(columns=['id', 'time', pos, 'speed_kf', 'run_index'])
    
    return leader_df

 
 




def extract_subject_and_leader_data(df, follower_id, run_index):
    sdf = df[(df['id'] == follower_id) & (df['run_index'] == run_index)].round(2)
    ldf = find_leader_data(df, follower_id, run_index).round(2)
    
    #find the intersection of time frames between leader and subject
    mutual_times = np.intersect1d(ldf['time'], sdf['time'])
    
    #find the longest continuous segment of mutual time
    max_continuous_mutual_times = []
    continuous_mutual_times = []
    prev_time = None
    for time in mutual_times:
        if prev_time is None or time - prev_time < 0.2:  #the time step is 0.1
            continuous_mutual_times.append(time)
        else:
            if len(continuous_mutual_times) > len(max_continuous_mutual_times):
                max_continuous_mutual_times = continuous_mutual_times
            continuous_mutual_times = [time]
        prev_time = time
    
    if len(continuous_mutual_times) > len(max_continuous_mutual_times):
        max_continuous_mutual_times = continuous_mutual_times
    
    #filter leader and subject data to include only the longest continuous mutual time
    ldf = ldf[ldf['time'].isin(max_continuous_mutual_times)]
    sdf = sdf[sdf['time'].isin(max_continuous_mutual_times)]
    
    if (isinstance(sdf, list) and not sdf) or (isinstance(sdf, pd.DataFrame) and sdf.empty):
        print(f"No subject data found for Follower ID {follower_id} and Run Index {run_index}.")
        empty_df = pd.DataFrame()
        return empty_df, empty_df
    
    else:
        start_time = sdf['time'].iloc[0]
        ldf['time'], sdf['time'] = ldf['time'] - start_time, sdf['time'] - start_time
        return sdf, ldf

 


def acceleration_calculator(i, vehicle_dict, time_headway, lambda_param, S_desired): 
    gap_error = vehicle_dict['gap'] + S_desired
    epsilon_dot = vehicle_dict['deltav'] 
    h = time_headway
    accl  = -(1 / h) * (epsilon_dot + lambda_param * gap_error)   
    return accl

 




def simulate_car_following(params):
    time_headway, dmin, lamb = params
    num_steps = round(total_time / time_step)
    time = np.linspace(0, total_time, num_steps)

    position = np.zeros(num_steps)
    speed = np.zeros(num_steps)
    acl = np.zeros(num_steps)

    position[0] = sdf.iloc[0][pos]
    speed[0] = sdf.iloc[0]['speed_kf']
    acl[0] = 0

    dt = time_step # time step  

    for i in range(1, num_steps):  
        S_desired = time_headway * speed[i - 1] + dmin # Compute desired spacing dynamically for CTH

        vehicle_dict = { 
            'gap':  position[i - 1] - leader_position[i - 1],
            'deltav': speed[i - 1] - leader_speed[i - 1],
            'speed': speed[i - 1],
            'time': time[i-1],
            'dt': dt
        }

        acceleration = acceleration_calculator(i, vehicle_dict, time_headway, lamb, S_desired) 
        speed[i] = speed[i - 1] + acceleration * dt
        position[i] = position[i - 1] + speed[i - 1] * dt + 0.5 * acceleration * (dt ** 2)

    return position, speed, acl





 
def fitness(params):
    sim_position, sim_speed, sim_accel = simulate_car_following(params) 
    diff_speed = np.array(sim_speed) - np.array(target_speed)    

   
    speed_deviation_penalty = np.sum(np.abs(diff_speed) ** 2)  

    
    mse_speed = np.mean(diff_speed ** 2)
    rmse_speed = np.sqrt(mse_speed)
    mae_speed = np.mean(np.abs(diff_speed))


    valid_speed_mask = np.array(target_speed) != 0
    mape_speed = (
        np.mean(np.abs(diff_speed[valid_speed_mask] / np.array(target_speed)[valid_speed_mask])) * 100 
        if np.any(valid_speed_mask) 
        else 0
    )

    speed_range = np.max(target_speed) - np.min(target_speed)
    nrmse_speed = rmse_speed / (speed_range if speed_range != 0 else 1)

    sse_speed = np.sum(diff_speed ** 2)
    ss_tot_speed = np.sum((target_speed - np.mean(target_speed)) ** 2)
    r2_speed = 1 - (sse_speed / ss_tot_speed) if ss_tot_speed != 0 else 0

    total_diff = np.sum(np.abs(diff_speed))

  
    fitness_value = 1.0 / (speed_deviation_penalty + 1e-6)

 
    error_metrics = {
        'MSE': mse_speed,
        'RMSE': rmse_speed,
        'MAE': mae_speed,
        'MAPE': mape_speed,
        'NRMSE': nrmse_speed,
        'SSE': sse_speed,
        'R-squared': r2_speed,
        'Total Difference': total_diff
    }

    return fitness_value, error_metrics







def crossover(parent1, parent2, param_ranges):
    crossover_point = random.randint(0, len(parent1) - 1)
    child1 = parent1[:crossover_point] + parent2[crossover_point:]
    child2 = parent2[:crossover_point] + parent1[crossover_point:]
 

    return child1, child2

 


def mutate(child, param_ranges):
    for i in range(len(child)):
        if random.random() < mutation_rate:
            child[i] += random.uniform(-0.1, 0.1) 
    return child


 

def genetic_algorithm(): 
    th_range = (th_min, th_max)
    dmin_range = (dmin_min, dmin_max)
    lamb_range = (lamb_min, lamb_max)   
    
    # Define parameter ranges
    param_ranges = [th_range, dmin_range, lamb_range]
 

    # Initialize population with random parameter values
    population = [[random.uniform(*range_) for range_ in param_ranges] for _ in range(population_size)]
    
    best_error = float('inf')
    best_individual = None
    best_metrics = None
    
    for generation in range(num_generations):
        # Evaluate fitness (simulate car-following behavior)
        fitness_and_errors = [fitness(individual) for individual in population]
        population_sorted = sorted(zip(population, fitness_and_errors), key=lambda x: x[1][0], reverse=True)
        population = [ind for ind, _ in population_sorted]
        
        # Update best individual if a better one is found
        current_best_error = population_sorted[0][1][1]['Total Difference']

        # Update best individual
        if current_best_error < best_error:
            best_error = current_best_error
            best_individual = population_sorted[0][0]
            best_metrics = population_sorted[0][1][1]

        # Parent selection
        parents = population[:len(population) // 2]
        children = []

        while len(children) < (population_size - len(parents)):
            parent1, parent2 = random.sample(parents, 2)
            child1, child2 = crossover(parent1, parent2, param_ranges)
            children.extend([mutate(child1, param_ranges), mutate(child2, param_ranges)])


           

        population = parents + children[:population_size - len(parents)]

    # Return best parameters and performance metrics
    best_individual = [max(value, 1e-6) for value in best_individual]
    return best_individual, best_error, best_metrics



 
 




def plot_simulation(timex, leader_position, target_position, sim_position, leader_speed, target_speed, sim_speed, follower_id, most_leading_leader_id, run_index, save_dir):
    plt.figure(figsize=(10, 12))
    plt.subplot(2, 1, 1)
    plt.plot(timex, leader_position, label='Leader')
    plt.plot(timex, target_position, label='Target')
    plt.plot(timex, sim_position, label='Simulated Follower')
    plt.xlabel('time (sec)')
    plt.ylabel('Position (m)')
    plt.title(f'Position vs time, FID: {follower_id}, LID: {int(most_leading_leader_id)}, run: {run_index}')
    plt.legend()
    plt.grid(True)

    plt.subplot(2, 1, 2)
    plt.plot(timex, leader_speed, label='Leader')
    plt.plot(timex, target_speed, label='Target')
    plt.plot(timex, sim_speed, label='Simulated Follower')
    plt.xlabel('time (sec)')
    plt.ylabel('Speed (m/s)')
    plt.title(f'Speed vs time, FID: {follower_id}, LID: {int(most_leading_leader_id)}, run: {run_index}')
    plt.legend()
    plt.grid(True)

    plot_filename = os.path.join(save_dir, f'{outname}_FID_{follower_id}_LID_{int(most_leading_leader_id)}_run_{run_index}.png')
    plt.savefig(plot_filename)
    plt.close()


 


def visualize_parameter_distributions(all_params,save_dir,outname):
    param_names = ['th','dmin', 'lamb']
    num_params = len(param_names)
    
    # Convert list of lists into a 2D numpy array
    all_params_array = np.array(all_params)

    # Ensure array is 2D (even if all_params is 1D)
    if all_params_array.ndim == 1:
        all_params_array = all_params_array.reshape(-1, num_params)

    # Create histograms for each parameter
    fig, axs = plt.subplots(1, num_params, figsize=(20, 4))

    for i in range(num_params):
        axs[i].hist(all_params_array[:, i], bins=20, color='skyblue', edgecolor='black')
        axs[i].set_title(param_names[i])
        axs[i].set_xlabel('Value')
        axs[i].set_ylabel('Frequency')

    plt.tight_layout()
    plot_filename = os.path.join(save_dir, f'{outname}_hist.png')
    plt.savefig(plot_filename)

    # Create box plots for each parameter
    plt.figure(figsize=(10, 6))
    plt.boxplot(all_params_array, labels=param_names, patch_artist=True)
    plt.title('Distribution of PT Model Parameters')
    plt.ylabel('Value')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plot_filename = os.path.join(save_dir, f'{outname}_box.png')
    plt.savefig(plot_filename)




def format_speed(df):
    """
    Computes heading and decomposes speed into speed_x and speed_y for each vehicle ID.
    
    Args:
        df (pd.DataFrame): DataFrame containing 'id', 'time', 'xloc', 'yloc', and 'speed' columns.

    Returns:
        pd.DataFrame: Updated DataFrame with 'heading', 'speed_x', and 'speed_y' columns.
    """
    
    vehicle_ids = df['id'].unique()
    df = df.sort_values(by=['id', 'time']).copy() 

    for temp_id in vehicle_ids:
        # Filter data for the specific vehicle ID
        temp_data = df[df['id'] == temp_id].copy()
        
        # Compute differences in x and y
        temp_data['dx'] = temp_data['xloc_kf'].diff()
        temp_data['dy'] = temp_data['yloc_kf'].diff()
        
        # Compute heading (in radians)
        temp_data['heading'] = np.arctan2(temp_data['dy'], temp_data['dx'])
        
        # Compute speed_x and speed_y
        temp_data['speed_x'] = temp_data['speed_kf'] * np.cos(temp_data['heading'])
        temp_data['speed_y'] = temp_data['speed_kf'] * np.sin(temp_data['heading'])

        # Fill NaN values for first row
        temp_data.fillna(0, inplace=True)

        # Assign back to the original DataFrame
        df.loc[df['id'] == temp_id, ['dx', 'dy', 'heading', 'speed_x', 'speed_y']] = temp_data[['dx', 'dy', 'heading', 'speed_x', 'speed_y']]
    
    df['speed_kf'] = df['speed_x']
    return df

 


#Save directory for plots
save_dir = 'Results/01CTH/'

#iterate through each dataset and group
for df_key, df_path in datasets.items(): 

    df = pd.read_csv(df_path)
    df = df.sort_values(by='time')
    df['time'] = df['time'].round(1)
 

    if df_key == "df395":
        pos = "yloc_kf" 
    else:
        pos = "xloc_kf"
        
  
    
    if df_key == "df9094":    
        df = format_speed(df)     

    for group in groups[df_key]:
        # Define the current group
        outname = str("PT_")+str(group)
        AVs = eval(group)
        all_params = []
        params_list = []

        for data in AVs:
            follower_id, run_index = data
            sdf, ldf = extract_subject_and_leader_data(df, follower_id, run_index)

            # Discard the last 100 points for each trajectory
            last_filter = 200 
            if len(sdf) > last_filter:
                sdf = sdf.iloc[:-last_filter]
            if len(ldf) > last_filter:
                ldf = ldf.iloc[:-last_filter]
                
           
            # Check if sdf is empty
            if sdf.empty:
                print(f"No data found for Follower ID {follower_id} and Run Index {run_index}. Skipping...")
                continue
            else:
                total_time = len(ldf) * 0.1
                time_step, num_steps = 0.1, round(total_time / 0.1)
                timex = np.linspace(0, total_time, num_steps)
                leader_position, leader_speed = ldf[pos].tolist(), ldf['speed_kf'].tolist()
                target_position, target_speed = sdf[pos].tolist(), sdf['speed_kf'].tolist()
                best_params, best_error, best_metrics = genetic_algorithm()
                all_params.append(best_params)
                params_list.append([follower_id, run_index] + best_params + [best_error] + list(best_metrics.values()))
                 
                sim_position, sim_speed, acl = simulate_car_following(best_params)
                plot_simulation(timex, leader_position, target_position, sim_position, leader_speed, target_speed, sim_speed, follower_id, most_leading_leader_id, run_index, save_dir)
        
        visualize_parameter_distributions(all_params,save_dir,outname)
        metrics_names = list(best_metrics.keys()) 
        columns = ['Follower_ID', 'Run_Index', 'th','dmin', 'lamb', 'Error'] + metrics_names
        params_df = pd.DataFrame(params_list, columns=columns)
        params_df.to_csv(f"{save_dir}{outname}.csv", index=False)


 