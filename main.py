import numpy as np
import matplotlib.pyplot as plt
import random
import pandas as pd
import math
from scipy.stats import norm
import os

# IDs for different types of Vehicles extracted before
# I395_A = [[694, 1], [1416, 1], [1779, 1], [2342, 1], [2817, 1], [3330, 1], [3758, 1], [4261, 1], [4715, 1], [5385, 1], [5741, 1], [6398, 1], [7299, 1], [7832, 1], [8171, 1], [8692, 1], [8885, 1], [9409, 1], [9505, 1], [10150, 1], [10182, 1]]
# I395_S = [(7128, 1), (2364, 1), (1635, 1), (4138, 1), (3890, 1), (1333, 1), (5680, 1), (9553, 1), (9388, 1), (9454, 1), (4519, 1), (2662, 1), (4586, 1), (5289, 1), (1108, 1), (9914, 1), (7571, 1), (2874, 1), (9231, 1), (9229, 1), (5281, 1)]
# I395_L = [(532, 1), (638, 1), (7082, 1), (4386, 1), (10022, 1), (4579, 1), (8194, 1), (9008, 1), (9230, 1), (6328, 1), (7804, 1), (6901, 1), (7581, 1), (7581, 1), (6901, 1), (7731, 1), (3450, 1), (8412, 1), (8884, 1), (7334, 1), (5284, 1), (8029, 1), (9208, 1), (7731, 1)]
# I9094_A = [[5366, 1], [195, 2], [286, 3]]
# I9094_S = [(3799, 1), (174, 2), (232, 3)]
# I9094_L = [(5012, 1), (5266, 1), (442, 3)]
# I294l1_A = [[8, 1], [9, 1], [12, 1], [33, 3], [40, 3], [41, 3], [3, 7], [11, 7], [17, 7], [51, 8], [62, 8], [65, 8], [24, 9], [28, 9], [30, 9], [19, 11], [22, 11], [35, 11], [18, 19], [25, 19], [48, 20], [50, 20], [54, 20], [13, 21]]
# I294l1_S = [(727, 7), (5557, 8), (46, 3), (17, 1), (33, 1), (65, 1), (24, 3), (575, 3), (628, 3),(15, 7), (24, 7), (67, 7), (81, 8), (145, 8), (197, 8), (32, 9), (48, 9), (778, 9), (38, 11), (132, 11), (497, 11), (321, 19), (1222, 19),(40, 20),(65, 20),(600, 20),(18, 21)]
# I294l1_L = [(603, 11), (1115, 20), (4681, 8), (18, 1), (116, 1),(202, 1), (7, 3), (30, 3),(65, 3), (26, 7), (34, 7), (128, 7), (45, 8), (93, 8), (539, 8), (462, 9), (2008, 9), (3062, 9), (12, 11), (39, 11), (121, 11), (4, 19), (108, 19), (55, 20), (806, 20), (1731, 20), (34, 21)]
# I294l2_A = [[462, 5], [107, 23], [291, 28], [90, 29], [118, 30], [231, 31], [181, 33], [218, 35], [46, 36], [72, 38], [211, 41], [229, 42]]
# I294l2_S = [[143, 5], [18, 23], [234, 28], [38, 29], [81, 30], [352, 31], [317, 33], [2, 35], [176, 36], [292, 38], [89, 41], [299, 42]]
# I294l2_L = [(260, 5), (111, 23), (156, 28), (102, 29), (30, 30), (35, 31), (25, 33), (8, 35), (71, 36), (86, 38), (364, 41), (9, 42)]

# Filter out only for ACC models 

'''
Instructions 
1. Take one vehicle input (ACC Level 1 and 2 cars) 
2. Find the vehicle infront of it because you need delta v, spacing and whatever parameters 
3. You need every variable the model needs 
'''
 




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


## MODEL PART WHICH NEEDS TO BE MODIFIED
# We are using the CONTROL LAW
# This is prospect theory model  
def acceleration_calculator(i, t, vehicle, accl_max, v_desired, Gamma1, Gamma2, Wm, Wc, Tmax, Alpha, Beta, Tcorr, RT, prng):
    So_D = 3 #default value by Talebpour
    if (vehicle['gap'] - So_D) > 0.1:
        Seff = vehicle['gap'] - So_D
    else:
        Seff = 0.1 #default value by Talebpour

    if vehicle['deltav'] > (Seff / Tmax):
        Tau = Seff / vehicle['deltav']
    else:
        Tau = Tmax

    if vehicle['deltav'] == 0:
        vehicle['deltav'] = 0.0000001 #default value by Talebpour
    if Alpha == 0:
        Alpha = 0.0000001 #default value by Talebpour
    Zprime = Tau / (2.0 * Alpha * vehicle['speed'])
    Zdoubleprime = 0.0

    #if Wc * Zprime >= 1:
    if Wc * Zprime > 0:
        if (2.0 * math.log(Wc * Zprime)) >= 0:
            a0 = 1
            #Zstar = (-1 * math.sqrt(2.0 * math.log(Wc * Zprime))) / (math.sqrt(2.0 * math.pi)) #default by Talebpour
            Zstar = -math.sqrt(2.0 * math.log((a0 * Wc * Tau) / (2.0 * math.sqrt(2.0 * math.pi) * Alpha * vehicle['speed']))) #changed to be consistent with paper
            if np.abs(Zstar) > 0.05:
                Zstar = 0.05 #added threshold to reduce fluctuations 
    else:
        Zstar = 0.0
    Astar = (2.0 / Tau) * ((Seff / Tau) - vehicle['deltav'] + (Alpha * vehicle['speed'] * Zstar))
    for NewtonCounter in range(3):
        X = Astar 
        if X >= 0:
            if X == 0:
                X = 0.0000001 #default value by Talebpour
            Uptprime = Gamma1 * math.pow(X, Gamma1 - 1)
            Uptdoubleprime = Gamma1 * (Gamma1 - 1) * math.pow(X, Gamma1 - 2)
        else:
            Uptprime = Wm * Gamma2 * pow(-X, Gamma2 - 1)
            Uptdoubleprime = -Wm * Gamma2 * (Gamma2 - 1) * pow(-X, Gamma2 - 2)

        Z = (vehicle['deltav'] + (0.5 * Astar * Tau) - (Seff / Tau)) / (Alpha * vehicle['deltav'])
        fn = norm.cdf(Z)

        F = Uptprime - Wc * fn * Zprime
        Fprime = Uptdoubleprime - Wc * fn * (Z * math.pow(Zprime, 2.0) + Zdoubleprime)
        if Fprime == 0:
            Fprime = 0.000000000001 #default value by Talebpour

        Astar = Astar - (F / Fprime)

    X = Astar
    if X >= 0:
        Uptprime = Gamma1 * math.pow(X, Gamma1 - 1)
        Uptdoubleprime = Gamma1 * (Gamma1 - 1) * math.pow(X, Gamma1 - 2)
    else:
        Uptprime = Wm * Gamma2 * pow(-X, Gamma2 - 1)
        Uptdoubleprime = -Wm * Gamma2 * (Gamma2 - 1) * pow(-X, Gamma2 - 2)
    Z = (vehicle['deltav'] + (0.5 * Astar * Tau) - (Seff / Tau)) / (Alpha * vehicle['deltav'])
    fn = norm.cdf(Z)
    F = Uptprime - Wc * fn * Zprime
    Fprime = Uptdoubleprime - Wc * fn * (Z * math.pow(Zprime, 2.0) + Zdoubleprime)
    if Fprime == 0:
        Fprime = 0.000000000001

    Var = -1.0 / (Beta * Fprime)
    
    Random_Wiener = np.random.rand()
    Yt = math.exp(-1 * 0.1 / Tau) + math.sqrt(24.0 * 0.1 / Tau) * Random_Wiener #default value by Talebpour
    accl_cf = Astar + Var * Yt
    accl_ff = accl_max * (1 - (vehicle['speed'] / v_desired))

    accl_ = np.minimum(accl_cf, accl_ff)

    if accl_ > 3: #default value by Talebpour
        accl_ = 3
    elif accl_ < -8: #default value by Talebpour
        accl_ = -8
    
    return accl_, fn, Wc * fn


def simulate_car_following(params):
    global Tmax, Alpha, Beta, Wc, Gamma1, Gamma2, Wm
    Tmax, Alpha, Beta, Wc, Gamma1, Gamma2, Wm = params
    
    num_steps = round(total_time / time_step)
    time = np.linspace(0, total_time, num_steps)
    
    position = np.zeros(num_steps)
    speed = np.zeros(num_steps)
    acl = np.zeros(num_steps)
    
    position[0] = sdf.iloc[0][pos]
    speed[0] = sdf.iloc[0]['speed_kf']
    acl[0] = 0

    for i in range(1, num_steps):
        dt = time_step
        desired_position = position[i - 1] + speed[i - 1] * dt
        
        acceleration, _, _ = acceleration_calculator(i, time[i], {'gap': leader_position[i-1] - position[i-1], 'deltav': leader_speed[i-1] - speed[i-1], 'speed': speed[i-1], 'vehID': follower_id}, accl_max, v_desired, Gamma1, Gamma2, Wm, Wc, Tmax, Alpha, Beta, Tcorr, RT, np.random.default_rng())

        acl[i] = acceleration
        speed[i] = speed[i - 1] + acceleration * dt
        position[i] = position[i - 1] + speed[i-1] * dt + 0.5 * acceleration * (dt**2)
        
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
    mape_speed = np.mean(np.abs(diff_speed / np.array(target_speed))) * 100
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
            child[i] += random.uniform(-0.1, 0.1)
    return child



def genetic_algorithm():
    # define parameter ranges for PT model
    Tmax_range = (2, 8.0)
    Alpha_range = (0, 0.6)
    Beta_range = (2, 8)
    Wc_range = (60000, 130000)
    Gamma1_range = (0.3, 2.0)
    Gamma2_range = (0.3, 2.0)
    Wm_range = (2, 8.0)

    #population with random parameter values
    population = [[random.uniform(*range_) for range_ in (Tmax_range, Alpha_range, Beta_range, Wc_range, Gamma1_range, Gamma2_range, Wm_range)]
                  for _ in range(population_size)]
    
    best_error = float('inf')
    best_individual = None
    best_metrics = None
    
    for generation in range(num_generations):
        #evaluate fitness and errors
        fitness_and_errors = [fitness(individual) for individual in population]
        population_sorted = sorted(zip(population, fitness_and_errors), key=lambda x: x[1][0], reverse=True)
        population = [ind for ind, _ in population_sorted]
        
        #Update best individual and best error if a better one is found
        current_best_error = population_sorted[0][1][1]['Total Difference']  # Error is the second element of the fitness_and_errors tuple
        if current_best_error < best_error:
            best_error = current_best_error
            best_individual = population_sorted[0][0]
            best_metrics = population_sorted[0][1][1]  # Best error metrics
        
        #Parent selection (top half of the sorted population)
        parents = population[:len(population) // 2]
        
        children = []
        while len(children) < (population_size - len(parents)):
            parent1, parent2 = random.sample(parents, 2)
            child1, child2 = crossover(parent1, parent2)
            children.extend([mutate(child1), mutate(child2)])
        population = parents + children[:population_size - len(parents)]
    
    #return the best individual, best error, and best error metrics after all generations
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

def visualize_parameter_distributions(all_params):
    param_names = ['Tmax', 'Alpha', 'Beta', 'Wc', 'Gamma1', 'Gamma2', 'Wm']
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
    plt.show()

    #create box plots for each parameter
    plt.figure(figsize=(10, 6))
    plt.boxplot(all_params_array, labels=param_names, patch_artist=True)
    plt.title('Distribution of PT Model Parameters')
    plt.ylabel('Value')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()



def main():
    population_size, num_generations, mutation_rate = 40, 80, 0.1  #simulation parameters
    accl_max, v_desired, Tcorr, RT = 3.0, 36.0, 20.0, 0.6 #suggested values from the paper and v_desired=36 is the v_desired from the data
    most_leading_leader_id = None


    
    # datasets = {
    # "df395": "TGSIM/I395_Trajectories.csv",
    # "df9094": "TGSIM/I90_94_Moving_Trajectories.csv",
    # "df294l1": "TGSIM/I294_L1_Trajectories.csv",
    # "df294l2": "TGSIM/I294_L2_Trajectories.csv"}

    # groups = {
    #     "df395": ["I395_A", "I395_S", "I395_L"],
    #     "df9094": ["I9094_L", "I9094_S", "I9094_A"],
    #     "df294l1": ["I294l1_L", "I294l1_S", "I294l1_A"],
    #     "df294l2": ["I294l2_L", "I294l2_S", "I294l2_A"]}

    datasets = { 
    "df9094": "TGSIM/I90_I94_Moving_Trajectories.csv",
    "df294l1": "TGSIM/I294_L1_Trajectories.csv",
    "df294l2": "TGSIM/I294_L2_Trajectories.csv"}

    groups = { 
        "df9094": ["I9094_L", "I9094_S", "I9094_A"],
        "df294l1": ["I294l1_L", "I294l1_S", "I294l1_A"],
        "df294l2": ["I294l2_L", "I294l2_S", "I294l2_A"]}


    #Save directory for plots
    save_dir = 'Results/'

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
                print (follower_id)
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
                    #print (params_list)
                    sim_position, sim_speed, acl = simulate_car_following(best_params)
                    plot_simulation(timex, leader_position, target_position, sim_position, leader_speed, target_speed, sim_speed, follower_id, most_leading_leader_id, run_index, save_dir)
            
            visualize_parameter_distributions(all_params)
            metrics_names = list(best_metrics.keys())
            columns = ['Follower_ID', 'Run_Index', 'Tmax', 'Alpha', 'Beta', 'Wc', 'Gamma1', 'Gamma2', 'Wm', 'Error'] + metrics_names
            params_df = pd.DataFrame(params_list, columns=columns)
            params_df.to_csv(f"{save_dir}{outname}.csv", index=False)


if __name__ == "__main__":
    main()