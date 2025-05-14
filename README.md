Final project for CSCI-GA 

To run this project, the only library required is `openssl`, to install:
- `brew install openssl@3`

After cloning the repo, run `make` in the root directory, there'll be two files compiled, `STARK_interactive` requires interaction from the verifier (you) via standard input, while `STARK_witness` would directly output a transcript in string.

There's also an implementation based on Szepieniec's python implementation, to run it, go to `src/py_impl/` and run `python3 LC3.py`

Many thanks to [Dr. Alan Szepieniec](https://asz.ink/about/), his tutorial helped me a lot [stark anatomy](https://aszepieniec.github.io/stark-anatomy/).

