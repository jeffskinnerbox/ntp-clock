<!--
Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
Version:      0.5.0
-->

# Managing Your Git Repository and GitHub
In here are instructions on the creation, maintenance, and use of this repository
via [git][01] and [GitHub][02].

Use [three simple rules][05] when using Git / Github:

1. **Rule #1: Create a Git repository for every new project** -
Every time you start working on something new, you should create a new Git repository and push it to GitHub.
1. **Rule #2: Create a new branch for every new feature** -
When creating a new feature, create a dedicated branch for this new feature,
give it a meaningful name, and commit all the code to that specific branch.
1. **Rule #3: Use Pull Requests to merge code to Master** -
You should never push changes directly to the master branch.
Instead, you should use feature branches as described above,
and open a new Pull Request to merge the feature branch code with the master branch code.

As an example, consider a repository called 'project'.
You create the inital local repository, push it to GitHub, make some modifications locally under a branch called 'doc-improvements', and they murge that branch with the master:

```bash
# ---------------------- Rule #1 ----------------------

# create your code and check it into a git local repository
<create your code for 'project'>
git init
git add --all
git commit -m 'creation initial repository'

# create your remote GetHub repository
<use 'New' button in repository directory on GitHub>

# set your remote repository URL
git remote add origin https://github.com/jeffskinnerbox/<project>.git

# pushes the changes from your local repository up to the remote repository
# branches colored in red have not been pulled down from the remote
git push -u origin master

# ---------------------- Rule 2 ----------------------

# create and checkout new branch called 'doc-improvements'
# could be done via single command: 'git checkout -b doc-improvements'
git branch doc-improvements
git checkout doc-improvements

# make you updates within the 'doc-improvements' branch
<make your code updates for 'project' within 'doc-improvements' branch>

# ---------------------- Rule 3 ----------------------

# push you changes to GetHub for the 'doc-improvement' branch
git add --all
git commit -m 'creation initial repository'
get push

# merge 'doc-improvements' branch with the 'master' branch
git checkout master
git merge doc-improvements

# show all local and remote branches
git branch -a
```

----

## Getting Started - Green Field
Lets assuming your starting without any code,
and your have not cloning the existing repository on GitHub.
Your creating the code from scratch or cut&paste from some source.
How do you start?

For this situation,
you'll need to do the following things to initialize your Git repositories.

### Step 1: Creating the Initial Remote Repository on GitHub
Go to [GitHub][02] and create the new repository called `ntp-clock`.
To avoid errors, do not initialize the new repository with `README.md`, license, or `.gitignore` files.
You can add these files when you push your project to GitHub.

### Step 2: Creating the Initial Local Git Repository
Make the `~/src/ntp-clock` directory, move into it, and initialize it as a git repository.

```bash
cd ~
mkdir ~/src/ntp-clock
cd ~/src/ntp-clock
git init
```

Also create the file `.gitignore` like this:

```
# Maintainer:   jeffskinnerbox@yahoo.com / www.jeffskinnerbox.me
# Version:      0.0.0
#
# DESCRIPTION:
# This file specifies intentionally untracked files that Git should ignore.
#
# Each line in this .gitignore file specifies a regular expression pattern
# that, when matched, the coresponding file will NOT be include in Git processing.



### --------------------------- Project Specific --------------------------- ###
credentials.json


### ------------------------------- General -------------------------------- ###

### Compiler and Linker Files ###
*.pyc
*.com
*.class
*.dll
*.exe
*.o
*.so
*.bin
*.elf

### Packages ###
*.7z
*.dmg
*.gz
*.iso
*.jar
*.rar
*.tar
*.zip

### Unit test / Coverage Reports
htmlcov/
.tox/
.coverage
.cache
nosetests.xml
coverage.xml

### Logs & Databases ###
*.log
*.sql
*.sqlite
.ipynb_checkpoints
.ropeproject
__pycache__

### OS Generated Files ###
*.out
*.swp
.DS_Store
.DS_Store?
._*
.Spotlight-V100
.Trashes
Icon?
ehthumbs.db
Thumbs.db
```

## Step 3: Create the git-secret Local Repository
The `ntp-clock` code needs the credentials for your local WiFi to work.
Check out the `GIT-SECRET.md` file for more information.

Initialize the `git-secret` repository by running `git secret init` command in your target project directory.
The `.gitsecret/` folder will be created.

>**NOTE:** All the contents of the `.gitsecret/` folder will need to be checked into your git repository,
>with the **exception** of the `random_seed` file.
>In other words, for the directory `.gitsecret`,
>only the random_seed file should be mentioned in your `.gitignore` file.
>`git-secret` will do all this automatically.

```bash
# go to your target directory
cd ~/src/ntp-clock

# create the git-secret's directory
git secret init
```

### Step 4: Add Users Who Can Access Secrets
Add the first user (yourself) to the `git-secret` repo keyring by running:

```bash
# add yourself to the git-secret keyring
$ git secret tell jeffskinnerbox@yahoo.com
```

>**NOTE:** This assumes you have created a gpg RSA key-pair (i.e. public and secret key)
>assoicated with the email address your using with your `git` repository.

### Step 5: Add Credentials File and Encrypt
Now add files you wish to encrypt inside the `git-secret` repository.
It can be done by running `git secret add <filenames...>` command.
`git-secret` automatically doesn't allow you to add the secret file to the repository.

After you add all the files you wish to encrypt,
run `git secret hide` to encrypt all files.
The data will be encrypted with the public-keys described by
the `git secret tell your@gpg.email` command
and placed in a file called `<filename>.sceret`.

```bash
# encrypt file via git-secret
git secret add credentials.json

# now encrypt the hidden files
$ git secret hide
git-secret: done. 1 of 1 files are hidden.
```

After using `git secret hide` to encrypt your data,
it is safe to commit your changes.

### Step 6: Commit All Files to Local Repository
Now commit the files to the `ntp-clock` git repository:

```bash
git status
git add --all
git commit -m 'Initial creation of ntp-clock repository'
```

### Step 7: Loading the GitHub Repository for the First Time
Within the `~/src/ntp-clock` directory, use `git` to load the files to GitHub,
assuming you're using public keys to create remote repository.

```bash
cd ~/src/ntp-clock
git remote add origin https://github.com/jeffskinnerbox/ntp-clock.git
git push -u origin master
```

Alternative is to login into GitHub, create the remote repository,
and create the repository manually.

----

## Getting Started - Clone GitHub
If you're cloning the GitHub repository `https://github.com/jeffskinnerbox/ntp-clock`,
there a few simple steps to get you going with `ntp-clock`.

### Step 1: Clone the Repository

```bash
# clone the repository
cd ~/src
git clone https://github.com/jeffskinnerbox/ntp-clock.git
```

### Step 2: Retrieving the Credentials
You can decrypt the credentials files with the `git secret reveal` command:

```bash
# retrieve the credentials
git secret reveal
```

If you used a password on your GPG key ring,
it will ask you for your password.

### Step 1:

----

### Updating the Local Git Repository
Within the .vim directory, do a "get status" to see what will be included in the commit,
add files (or remove) that are required, and then do the commit to the local git repository.

    git status
    git add --all
    git commit --dry-run
    git commit -m <comment>

#### Store Credentials Within Git
To add a new remote,
use the `git remote add` command on the terminal,
in the directory your repository is stored at.

    cd ~/jupyter-notebooks

    # set your remote repository URL
    git remote add origin https://github.com/jeffskinnerbox/jupyter-notebooks.git

    # verifies the new remote URL
    git remote -v

    # pushes the changes in your local repository up to the remote repository
    git push -u origin master

>**NOTE**: Other operations
[rename an existing remote](https://help.github.com/articles/renaming-a-remote/),
[delete an existing remote](https://help.github.com/articles/removing-a-remote/).


### Updating the Remote Repository (i.e. GitHub)
To which shows you the URL that Git has stored for the shortname for
the remote repository (i.e. GitHub):

    git remote -v

Now to push your files to the GitHub repository

    git push -u origin master

### Retrieving Update From Remote Repository (i.e. GitHub)
To retrieve these updates on another system, use

    git pull origin master

To overwrite everything in the local directory

    git fetch --all
    git reset --hard origin/master

Explanation: `git fetch` downloads the latest from remote without trying to merge or rebase anything.
Then the `git reset` resets the master branch to what you just fetched.
The `--hard` option changes all the files in your working tree to match the files in `origin/master`.
If you have any files that are _not_ tracked by Git,
these files will not be affected.

----

## Updating a Git Repository

### Updating the Local Git Repository
Within the .vim directory, do a "get status" to see what will be included in the commit,
add files (or remove) that are required, and then do the commit to the local git repository.

    git status
    git add --all
    git commit --dry-run
    git commit -m <comment>

### Retrieving Update From Remote Repository (i.e. GitHub)
To retrieve these updates on another system, use

    git pull origin master

To overwrite everything in the local directory

    git fetch --all
    git reset --hard origin/master

Explanation: `git fetch` downloads the latest from remote without trying to merge or rebase anything.
Then the `git reset` resets the master branch to what you just fetched.
The `--hard` option changes all the files in your working tree to match the files in `origin/master`.
If you have any files that are _not_ tracked by Git,
these files will not be affected.

### Updating the Remote Repository (i.e. GitHub)
To which shows you the URL that Git has stored for the shortname for
the remote repository (i.e. GitHub):

    git remote -v

Now to push your files to the GitHub repository

    git push -u origin master

================================================================================

## Cloning a Git Repository

### Clone This Git Repository
Copy this Git repository into your local systems:

================================================================================

time to install my personal tools on another device.

```bash
# execute the pyenv installer
curl -L https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash

# tool to check for window manager
sudo apt-get install wmctrl

# setup your vim environment
cd ~
git clone https://github.com/jeffskinnerbox/.vim.git
ln -s ~/.vim/vimrc ~/.vimrc
mkdir ~/.vim/backup
mkdir ~/.vim/tmp
cd ~/.vim
git submodule init
git submodule update

# install X configuration files
cd ~
git clone https://github.com/jeffskinnerbox/.X.git
ln -s ~/.X/xbindkeysrc ~/.xbindkeysrc
ln -s ~/.X/Xresources ~/.Xresources
ln -s ~/.X/xsessionrc ~/.xsessionrc

# rebuilding $HOME/.Xauthority to avoid MIT magic cookie error
touch ~/.Xauthority
xauth generate :0 . trusted
xauth add ${HOST}:0 . `xxd -l 16 -p /dev/urandom`

# install tools for bash shell
cd ~
rm ~/.bashrc ~/.bash_logout
git clone https://github.com/jeffskinnerbox/.bash.git
ln -s ~/.bash/bashrc ~/.bashrc
ln -s ~/.bash/bash_login ~/.bash_login
ln -s ~/.bash/bash_logout ~/.bash_logout
ln -s ~/.bash/bash_profile ~/.bash_profile
ln -s ~/.bash/dircolors.old ~/.dircolors

# setup pyenv by executing the pyenv installer
curl -s https://raw.githubusercontent.com/pyenv/pyenv-installer/master/bin/pyenv-installer | bash
pyenv install 3.6.4         # install python 3.6.4 via pyenv
pyenv rehash                # assure the pyenv shims are updated
```



[01]:http://git-scm.com/
[02]:https://github.com/
[03]:http://blog.smalleycreative.com/tutorials/using-git-and-github-to-manage-your-dotfiles/
[04]:http://blog.sanctum.geek.nz/managing-dot-files-with-git/
[05]:https://medium.freecodecamp.org/follow-these-simple-rules-and-youll-become-a-git-and-github-master-e1045057468f
[06]:
[07]:
[08]:
[09]:
[10]:
