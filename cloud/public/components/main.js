async function Main(resolve) {
    const response = await axios.get('components/main.html');
    resolve({
        template: response.data,
        data: function () {
            return {
                shared: shared,
                grid: 0,
                pv: 0,
                lastUpdated: '',
                selectDate: '',
                chart: null,
                data: [
                    [],
                    []
                ],
                chart2: null,
                data2: [
                    [],
                    []
                ],
                password: '',
                isShowPassword: false
            };
        },
        mounted: function () {
            shared.detachListener1();
            shared.detachListener2();

            auth.onAuthStateChanged((user) => {
                if (!user) {
                    location = '#/';
                }
            });

            shared.detachListener1();
            shared.detachListener1 = firestore.collection('logs')
                .orderBy('created', 'desc')
                .limit(1)
                .onSnapshot((querySnapshot) => {
                    querySnapshot.forEach((doc) => {
                        this.grid = doc.data().grid.toLocaleString();
                        this.pv = doc.data().pv.toLocaleString();
                        this.lastUpdated = doc.data().created.toDate().toLocaleString();
                    });
                });

            const dt = new Date();
            let m = dt.getMonth() + 1;
            if (m < 10) m = '0' + m;
            let d = dt.getDate();
            if (d < 10) d = '0' + d;
            this.selectDate = dt.getFullYear() + '-' + m + '-' + d;

            for (let i = 0; i < 72; i++) {
                this.data[0].push(0);
                this.data[1].push(0);
                this.data2[0].push(0);
                this.data2[1].push(0);
            }

            const labels = [];
            labels.push('12:00 am');
            for (let j = 0; j < 5; j++) {
                labels.push('12:' + (j + 1) + '0 am');
            }
            for (let i = 0; i < 11; i++) {
                labels.push((i + 1) + ':00 am');
                for (let j = 0; j < 5; j++) {
                    labels.push((i + 1) + ':' + (j + 1) + '0 am');
                }
            }

            this.chart = new Chart(this.$refs.chart.getContext('2d'), {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Grid',
                        borderColor: 'red',
                        data: this.data[0]
                    }, {
                        label: 'Solar PV',
                        borderColor: 'green',
                        data: this.data[1]
                    }]
                },
                options: {
                    aspectRatio: 1.25
                }
            });

            const labels2 = [];
            labels2.push('12:00 pm');
            for (let j = 0; j < 5; j++) {
                labels2.push('12:' + (j + 1) + '0 pm');
            }
            for (let i = 0; i < 11; i++) {
                labels2.push((i + 1) + ':00 pm');
                for (let j = 0; j < 5; j++) {
                    labels2.push((i + 1) + ':' + (j + 1) + '0 pm');
                }
            }

            this.chart2 = new Chart(this.$refs.chart2.getContext('2d'), {
                type: 'line',
                data: {
                    labels: labels2,
                    datasets: [{
                        label: 'Grid',
                        borderColor: 'red',
                        data: this.data2[0]
                    }, {
                        label: 'Solar PV',
                        borderColor: 'green',
                        data: this.data2[1]
                    }]
                },
                options: {
                    aspectRatio: 1.25
                }
            });

            this.selectDateChange();
        },
        methods: {
            logout: async function () {
                await auth.signOut();
            },
            selectDateChange: function () {
                for (let i = 0; i < 72; i++) {
                    this.data[0][i] = 0;
                    this.data[1][i] = 0;
                    this.data2[0][i] = 0;
                    this.data2[1][i] = 0;
                }

                shared.detachListener2();
                shared.detachListener2 = firestore.collection('logs')
                    .where('created', '>=', new Date(this.selectDate + ' 00:00:00'))
                    .where('created', '<=', new Date(this.selectDate + ' 23:59:59'))
                    .orderBy('created')
                    .limit(300)
                    .onSnapshot((querySnapshot) => {
                        querySnapshot.forEach((doc) => {
                            const grid = doc.data().grid;
                            const pv = doc.data().pv;
                            const lastUpdated = doc.data().created.toDate();
                            let index = lastUpdated.getHours() * 6 + Math.floor(lastUpdated.getMinutes() / 10);

                            let data;
                            if (index < 72) {
                                data = this.data;
                            } else {
                                data = this.data2;
                                index -= 72;
                            }

                            if (data[0][index] === 0) {
                                data[0][index] += grid;
                            } else {
                                data[0][index] += grid;
                                data[0][index] /= 2;
                            }

                            if (data[1][index] === 0) {
                                data[1][index] += pv;
                            } else {
                                data[1][index] += pv;
                                data[1][index] /= 2;
                            }
                        });
                        this.chart.update();
                        this.chart2.update();
                    });
            },
            showPassword: function () {
                this.isShowPassword = !this.isShowPassword;
            },
            saveNewPassword: async function () {
                if (!this.password) {
                    alert('Password required');
                    return;
                }
                try {
                    await auth.currentUser.updatePassword(this.password);

                    $(this.$refs.changePasswordModal).modal('hide');

                    this.password = '';
                    this.isShowPassword = false;

                    alert('Password changed');
                } catch (ex) {
                    alert(ex.message);
                }
            }
        }
    });
}
