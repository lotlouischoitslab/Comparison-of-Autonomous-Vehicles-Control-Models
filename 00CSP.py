import numpy as np
import matplotlib.pyplot as plt
import random
import pandas as pd
import math
from scipy.stats import norm
import os
 


datasets = {
"df395": "TGSIM/I395_Trajectories.csv",
"df9094": "TGSIM/I90_I94_Moving_Trajectories.csv",
"df294l1": "TGSIM/I294_L1_Trajectories.csv",
"df294l2": "TGSIM/I294_L2_Trajectories.csv"
}

 

groups = {
    "df395": ["I395_A"],
    "df9094": ["I9094_A"],
    "df294l1": ["I294l1_A"],
    "df294l2": ["I294l2_A"]
    }


I395_A, I9094_A, I294l1_A, I294l2_A = [], [], [], []


for data_key, data_path in datasets.items():
    temp_df = pd.read_csv(data_path) 
 

    if data_key == 'df395': 
        temp_df_av = temp_df[temp_df['type_most_common'] == 4]
        temp_df_id = temp_df_av['id'].unique()
        temp_df_run_index = temp_df_av['run_index'].values[0]


        for id_val in temp_df_id:
            I395_A.append([id_val,temp_df_run_index])


    elif data_key == 'df9094':
        temp_df_av = temp_df[temp_df['av'] == 'yes']
        temp_df_id = temp_df_av['id'].unique()
        temp_df_run_index = temp_df_av['run_index'].unique()
    
      
        for id_val, run_index_val in zip(temp_df_id, temp_df_run_index):
            I9094_A.append([id_val, run_index_val])


    elif data_key == 'df294l1':
        temp_df['acc'] = temp_df['acc'].str.lower()
        temp_df_av = temp_df[temp_df['acc'] == 'yes']
        temp_df_id = temp_df_av['id'].unique()
        temp_df_run_index = temp_df_av['run_index'].unique()


        for id_val, run_index_val in zip(temp_df_id, temp_df_run_index):
            I294l1_A.append([id_val, run_index_val])


    elif data_key == 'df294l2':
        temp_df['acc'] = temp_df['acc'].str.lower()
        temp_df_av = temp_df[temp_df['acc'] == 'yes']
        temp_df_id = temp_df_av['id'].unique()
        temp_df_run_index = temp_df_av['run_index'].unique()


        for id_val, run_index_val in zip(temp_df_id, temp_df_run_index):
            I294l2_A.append([id_val, run_index_val])



print(I395_A)
print(I9094_A)
print(I294l1_A)
print(I294l2_A)



# Simulation Parameters
population_size = 40
num_generations = 80 
mutation_rate = 0.2
delta =  0.005
accl_min = -1 
accl_max = 1 
most_leading_leader_id = None
kmin = 0.01
kmax = 0.9


def find_leader_data(df, follower_id, run_index):
    global most_leading_leader_id
    
    follower_data = df[(df['id'] == follower_id) & (df['run_index'] == run_index)]
    leader_data_dict = {}
    
    for index, row in follower_data.iterrows():
        time = row['time']
        follower_x = row[pos]
        follower_lane = row['lane_kf']
        run_index = row['run_index']

        #find the leader
        leader_data = df[(df['id'] != follower_id) & (df['time'] == time) & (df['lane_kf'] == follower_lane) & (df[pos] > follower_x) & (df['run_index'] == run_index)]
        
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
                                   'run_index': run_index})
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

 

  

def genetic_algorithm(): 
    # Add kv and kp parameter ranges
    kv_range = (kmin, kmax)  # Increased damping
    kp_range = (kmin, kmax)  # Reduced overreaction 

    # Define parameter ranges for each parameter 
    param_ranges = [kv_range, kp_range]

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
        if current_best_error < best_error:
            best_error = current_best_error
            best_individual = population_sorted[0][0]
            best_metrics = population_sorted[0][1][1]

        # Parent selection (top half of sorted population)
        parents = population[:len(population) // 2]

        # Crossover and mutation
        children = []
        while len(children) < (population_size - len(parents)):
            parent1, parent2 = random.sample(parents, 2)
            child1, child2 = crossover(parent1, parent2)
            children.extend([mutate(child1), mutate(child2)])
        
        # Ensure population size remains constant
        population = parents + children[:population_size - len(parents)]

    # Return best parameters and performance metrics
    return best_individual, best_error, best_metrics


 


def acceleration_calculator(i, t, vehicle, accl_min, accl_max, kv, kp):
    S_desired = 3
    v_desired = 36 
    """
    Implements a constant spacing policy using a PD controller and integrates free-flow acceleration.

    Args:
        i (int): Index of vehicle in the platoon.
        t (float): Current time step.
        vehicle (dict): Contains 'gap' (distance to lead vehicle), 'deltav' (speed difference), and 'speed' (ego speed).
        accl_min (float): Minimum allowable acceleration (deceleration limit).
        accl_max (float): Maximum allowable acceleration.
        kv (float): Derivative gain for speed control.
        kp (float): Proportional gain for gap control.
        S_desired (float): Desired constant spacing distance.
        v_desired (float): Desired free-flow velocity.

    Returns:
        float: Calculated acceleration with free-flow adjustment.
    """
    # Compute acceleration using PD controller to maintain spacing (Car-Following Model)
    gap_error = vehicle['gap'] + S_desired
    speed_error = vehicle['deltav']

    accl_cf = - kv * speed_error -kp * gap_error  # Car-following acceleration

    # Compute Free-Flow Acceleration (Tends to move towards desired speed)
    accl_ff = accl_max * (1 - (vehicle['speed'] / v_desired))

    # Final acceleration: Minimum of Car-Following and Free-Flow Acceleration
    accl = np.minimum(accl_cf, accl_ff) 
    accl = np.clip(accl, accl_min, accl_max)  # Using acceleration bounds  
 
    return accl


def simulate_car_following(params):
    kv, kp = params  


    """
    Simulates a vehicle following a lead car using a constant spacing policy.
    
    Args:
        params (tuple): Contains simulation parameters.
        kv (float): Proportional gain for maintaining rel vel. 
        kp (float): Proportional gain for maintaining spacing. 
         
    
    Returns:
        tuple: position, speed, and acceleration arrays.
    """ 
    
    num_steps = round(total_time / time_step)
    time = np.linspace(0, total_time, num_steps)
    
    position = np.zeros(num_steps)
    speed = np.zeros(num_steps)
    acl = np.zeros(num_steps)
    
    position[0] = sdf.iloc[0][pos]
    speed[0] = sdf.iloc[0]['speed_kf']
    
    for i in range(1, num_steps):
        dt = time_step
        vehicle_state = {
            'gap':  position[i - 1] - leader_position[i - 1],
            'deltav': speed[i - 1] - leader_speed[i - 1],
            'speed': speed[i - 1]
        }

        # Compute acceleration using the constant spacing policy
        acceleration = acceleration_calculator(i, time[i], vehicle_state,accl_min, accl_max, kv, kp)

        acl[i] = acceleration
        speed[i] = speed[i - 1] + acceleration * dt
        position[i] = position[i - 1] + speed[i - 1] * dt + 0.5 * acceleration * (dt ** 2)
        
    return position, speed, acl

 


def fitness(params):
    sim_position, sim_speed, acl = simulate_car_following(params)
    diff_position = np.array(sim_position) - np.array(target_position)
    diff_speed = np.array(sim_speed) - np.array(target_speed)
    
    # Calculate errors
    mse_position = np.mean(diff_position ** 2)
    mse_speed = np.mean(diff_speed ** 2)
    mse = mse_position + mse_speed
    
    rmse_position = np.sqrt(mse_position)
    rmse_speed = np.sqrt(mse_speed)
    rmse = np.sqrt(mse)
    
    mae_position = np.mean(np.abs(diff_position))
    mae_speed = np.mean(np.abs(diff_speed))
    mae = mae_position + mae_speed
    
    mape_position = np.mean(np.abs(diff_position / np.array(target_position))) * 100
    mape_speed = np.mean(np.abs(diff_speed / (np.array(target_speed) + 1e-8))) * 100
    mape = (mape_position + mape_speed) / 2
    
    nrmse_position = rmse_position / (np.max(target_position) - np.min(target_position))
    nrmse_speed = rmse_speed / (np.max(target_speed) - np.min(target_speed))
    nrmse = (nrmse_position + nrmse_speed) / 2
    
    sse_position = np.sum(diff_position ** 2)
    sse_speed = np.sum(diff_speed ** 2)
    sse = sse_position + sse_speed
    
    ss_res_position = np.sum(diff_position ** 2)
    ss_tot_position = np.sum((np.array(target_position) - np.mean(target_position)) ** 2)
    r2_position = 1 - (ss_res_position / ss_tot_position)
    
    ss_res_speed = np.sum(diff_speed ** 2)
    ss_tot_speed = np.sum((np.array(target_speed) - np.mean(target_speed)) ** 2)
    r2_speed = 1 - (ss_res_speed / ss_tot_speed)
    
    r2 = (r2_position + r2_speed) / 2
    
    total_diff = np.sum(np.abs(diff_position)) + np.sum(np.abs(diff_speed))
    
    # Fitness is the inverse of total error to maximize fitness
    fitness_value = 1.0 / (total_diff + 1e-5)
    
    # Store all error metrics in a dictionary
    error_metrics = {
        'MSE': mse,
        'RMSE': rmse,
        'MAE': mae,
        'MAPE': mape,
        'NRMSE': nrmse,
        'SSE': sse,
        'R-squared': r2,
        'Total Difference': total_diff
    }
    
    return fitness_value, error_metrics  # Return fitness and all error metrics


 




def crossover(parent1, parent2):
    crossover_point = random.randint(0, len(parent1) - 1)
    child1 = parent1[:crossover_point] + parent2[crossover_point:]
    child2 = parent2[:crossover_point] + parent1[crossover_point:]
    return child1, child2

 

def mutate(child):
    for i in range(len(child)):
        if random.random() < mutation_rate:
            mutation_value = random.uniform(-delta, delta)  # Small mutation step
            child[i] += mutation_value  # Apply mutation
 
    return child


# def mutate(child):
#     for i in range(len(child)):
#         if random.random() < mutation_rate:
#             mutation_value = random.uniform(-delta, delta)  
#             child[i] += mutation_value  

#             # Ensure kv and kp remain non-negative
#             if i == 0:  # `kv`
#                 child[i] = max(child[i], kmin)  
#             elif i == 1:  # `kp`
#                 child[i] = max(child[i], kmin)  

#     return child




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
    param_names = ['kv','kp']
    num_params = len(param_names)
    
    #convert list of lists into a 2D numpy array for easier column-wise access
    all_params_array = np.array(all_params)
    
    #histograms for each parameter
    fig, axs = plt.subplots(1, num_params, figsize=(20, 4))
    for i in range(num_params):
        axs[i].hist(all_params_array[:, i], bins=20, color='skyblue', edgecolor='black')
        axs[i].set_title(param_names[i])
        axs[i].set_xlabel('Value')
        axs[i].set_ylabel('Frequency')
    
    plt.tight_layout()
    plot_filename = os.path.join(save_dir, f'{outname}_hist.png')
    plt.savefig(plot_filename)

    #create box plots for each parameter
    plt.figure(figsize=(10, 6))
    plt.boxplot(all_params_array, labels=param_names, patch_artist=True)
    plt.title('Distribution of PT Model Parameters')
    plt.ylabel('Value')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plot_filename = os.path.join(save_dir, f'{outname}_box.png')
    plt.savefig(plot_filename)



 


#Save directory for plots
save_dir = 'Results/00CSP/'



#iterate through each dataset and group
for df_key, df_path in datasets.items():
    df = pd.read_csv(df_path)
    df = df.sort_values(by='time')
    df['time'] = df['time'].round(1)
    if df_key == "df395":
        pos = "yloc_kf"
    else:
        pos = "xloc_kf"

    for group in groups[df_key]:
        # Define the current group
        outname = str("PT_")+str(group)
        AVs = eval(group)
        all_params = []
        params_list = []

        for data in AVs:
            follower_id, run_index = data
            sdf, ldf = extract_subject_and_leader_data(df, follower_id, run_index)
            
    
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
        columns = ['Follower_ID', 'Run_Index','kv', 'kp', 'Error'] + metrics_names
        params_df = pd.DataFrame(params_list, columns=columns)
        params_df.to_csv(f"{save_dir}{outname}.csv", index=False)


 