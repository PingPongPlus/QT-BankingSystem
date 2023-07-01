import random

# Use this script instead of getBTCPrice.py to try investments with simulated prices.
def generate_random_number(min_val=1, max_val=110000):
    return random.randint(min_val, max_val)

if __name__ == "__main__":
    random_number = generate_random_number()
    print(random_number)
