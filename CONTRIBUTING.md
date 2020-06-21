I do accept open source contributions from people who do not participate
in my blog. Ideally though all contributions are 'optional extras'
or security fixes that won't hamper the future direction of
the database's evolution.

Branch naming conventions

- `trunk` - The main published branch (used to be called `master` or `main`). I like trees.
- `stem` - The development branch (not `develop`). So named because a stem is responsible for the production of new living tissue in a tree! I really like trees.
- `feature-ISSUENUMBER` - Feature branch off of `stem` only.

To contribute code:-
- Open an issue in GitHub for your feature
- Create a fork of the code
- Create a branch off of `stem` called `feature-ISSUENUMBER`
- Carry out all work here
- Ensure you follow TDD - code without tests will be rejected
- Ensure your tests all pass!
- Please submit pull requests to the `stem` branch from your feature branch

