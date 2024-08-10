
class StatTracker:
    def __init__(self, running_avg_new_sample_weight):
        self._running_avg_new_sample_weight = running_avg_new_sample_weight
        self._avg = 0

    def sample(self, new_sample):
        # Update running average
        self._avg = self._running_avg_new_sample_weight * new_sample + (1 - self._running_avg_new_sample_weight) * self._avg

    def get_running_avg(self):
        return self._avg