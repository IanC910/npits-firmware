
class RunningAvg:
    def __init__(self, running_avg_new_sample_weight, initial_val):
        self._running_avg_new_sample_weight = running_avg_new_sample_weight
        self._avg = initial_val

    def sample(self, new_sample):
        # Update running average
        self._avg = self._running_avg_new_sample_weight * new_sample + (1 - self._running_avg_new_sample_weight) * self._avg

    def get(self):
        return self._avg